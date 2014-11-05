/*
 * Morpheuz Sleep Monitor
 *
 * Copyright (c) 2013-2014 James Fowler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "pebble.h"
#include "analogue.h"
#include "morpheuz.h"

const GPathInfo MINUTE_HAND_POINTS = { 5, (GPoint[] ) { { -5, 8 }, { 5, 8 }, { 5, -40 }, { 0, -50 }, { -5, -40 }, } };

const GPathInfo HOUR_HAND_POINTS = { 5, (GPoint[] ) { { -5, 8 }, { 5, 8 }, { 5, -30 }, { 0, -40 }, { -5, -30 }, } };

static Layer *analgue_layer;

static GPath *minute_arrow;
static GPath *hour_arrow;
static Layer *hands_layer;
static BitmapLayer *logo_layer;
static GBitmap *logo_bitmap;
static struct PropertyAnimation* analogue_animation;
static BitmapLayer *alarm_layer;
extern GBitmap *alarm_bitmap;
static BitmapLayer *weekend_layer;
static GBitmap *weekend_bitmap;
static TextLayer *powernap_layer;
static TextLayer *text_12;
static TextLayer *text_3;
static TextLayer *text_6;
static TextLayer *text_9;

static bool show_smart_points;
static int16_t from_time;
static int16_t to_time;
static int16_t start_time;
static int16_t progress_1;
static int16_t progress_2;
static bool is_visible = false;
static char powernap_text[3];

/*
 * Show the alarm is set icon
 */
static void analogue_alarm_set(bool value) {
  layer_set_hidden(bitmap_layer_get_layer(alarm_layer), !value);
}

/*
 * Draws marks around the circumference of the clock face
 * inner = pixels innermost
 * outer = pixels outermost
 * start = 0 to 1440 starting position
 * stop = 0 to 1440 ending position
 * step = 120: hourly; 24: minute; etc
 * jitter = line thickening (low numbers less, high numbers more)
 */
static void draw_marks(Layer *layer, GContext *ctx, int inner, int outer, int start, int stop, int step, int jitter) {
  GRect bounds = layer_get_bounds(layer);
  const GPoint center = grect_center_point(&bounds);
  const int16_t furthestOut = bounds.size.w / 2;
  const int16_t pixelsInner = furthestOut - inner;
  const int16_t pixelsOuter = furthestOut - outer;

  GPoint pointInner;
  GPoint pointOuter;

  for (int i = start; i < stop; i += step) {
    for (int j = -jitter; j < jitter; j++) {

      int32_t second_angle = (TRIG_MAX_ANGLE * (i + j) / 1440);
      int32_t minus_cos = -cos_lookup(second_angle);
      int32_t plus_sin = sin_lookup(second_angle);

      pointInner.y = (int16_t) (minus_cos * (int32_t) pixelsInner / TRIG_MAX_RATIO) + center.y;
      pointInner.x = (int16_t) (plus_sin * (int32_t) pixelsInner / TRIG_MAX_RATIO) + center.x;
      pointOuter.y = (int16_t) (minus_cos * (int32_t) pixelsOuter / TRIG_MAX_RATIO) + center.y;
      pointOuter.x = (int16_t) (plus_sin * (int32_t) pixelsOuter / TRIG_MAX_RATIO) + center.x;

      graphics_draw_line(ctx, pointInner, pointOuter);
    }
  }

}

/*
 * Update the clockface layer if it needs it
 */
static void bg_update_proc(Layer *layer, GContext *ctx) {

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_context_set_stroke_color(ctx, GColorWhite);

  // Hours and minute marks
  draw_marks(layer, ctx, HOUR, CLOCK, 0, 1440, 120, 7);
  draw_marks(layer, ctx, MIN, CLOCK, 0, 1440, 24, 1);

  // Start and first and last times for smart alarm
  if (show_smart_points) {
    draw_marks(layer, ctx, OUTER_STOP, OUTER, from_time, from_time + 1, 1, 7);
    draw_marks(layer, ctx, OUTER_STOP, OUTER, to_time, to_time + 1, 1, 7);
  }

  // Show reset point
  if (start_time != -1) {
    draw_marks(layer, ctx, OUTER_STOP, OUTER, start_time, start_time + 1, 1, 7);

    // Progress line
    if (progress_1 != -1) {
      draw_marks(layer, ctx, PROGRESS_STOP, PROGRESS, start_time, progress_1, 1, 1);
      if (progress_2 != -1) {
        draw_marks(layer, ctx, PROGRESS_STOP, PROGRESS, 0, progress_2, 1, 1);
      }
    }
  }
}

/*
 * Record the smart times for display on the analogue clock. Trigger an update of the layer.
 */
void analogue_set_smart_times(bool smart, int8_t fromhr, int8_t frommin, int8_t tohr, int8_t tomin) {
  show_smart_points = smart;
  from_time = (fromhr > 12 ? fromhr - 12 : fromhr) * 120 + frommin * 2;
  to_time = (tohr > 12 ? tohr - 12 : tohr) * 120 + tomin * 2;
  if (is_visible)
    layer_mark_dirty(analgue_layer);
  analogue_alarm_set(smart);
}

/*
 * Record the base time for display on the analogue clock. Trigger an update of the layer.
 */
void analogue_set_base(time_t base) {
  if (base == 0) {
    start_time = -1;
  } else {
    struct tm *time = localtime(&base);
    start_time = (time->tm_hour > 12 ? time->tm_hour - 12 : time->tm_hour) * 120 + time->tm_min * 2;
  }
  if (is_visible)
    layer_mark_dirty(analgue_layer);
}

/*
 * Mark progress on the analogue clock. Progress 1-54. Trigger an update of the layer
 */
void analogue_set_progress(uint8_t progress_level_in) {
  progress_1 = start_time + ((int16_t)progress_level_in) * 20;
  if (progress_1 >= 1440) {
    progress_2 = progress_1 - 1440;
    progress_1 = 1439;
  } else {
    progress_2 = -1;
  }
  if (is_visible)
    layer_mark_dirty(analgue_layer);
}

/*
 * Plot the normal time display on the clock
 */
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, minute_arrow);
  gpath_draw_outline(ctx, minute_arrow);

  gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, hour_arrow);
  gpath_draw_outline(ctx, hour_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 2, bounds.size.h / 2 - 2, 5, 5), 1, GCornersAll);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornersAll);
}

/*
 * Trigger the refresh of the time
 */
void analogue_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  if (is_visible)
    layer_mark_dirty(hands_layer);
}

/**
 * Load the analogue clock watch face
 */
void analogue_window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);

  // Init internal state used by bg_update_proc
  show_smart_points = false;
  from_time = 0;
  to_time = 0;
  start_time = -1;
  progress_1 = -1;
  progress_2 = -1;

  // init layers
  analgue_layer = layer_create(ANALOGUE_START);
  layer_set_update_proc(analgue_layer, bg_update_proc);
  layer_add_child(window_layer, analgue_layer);

  logo_layer = bitmap_layer_create(GRect(144/2-25, 144/2-15-31+10, 50, 31));
  layer_add_child(analgue_layer, bitmap_layer_get_layer(logo_layer));
  logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SMALL_LOGO);
  bitmap_layer_set_bitmap(logo_layer, logo_bitmap);

  text_12 = macro_text_layer_create(GRect(144/2-14, 21, 20, 32),analgue_layer,GColorWhite, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentRight);
  text_layer_set_text(text_12, TEXT_12);

  text_3 = macro_text_layer_create(GRect(144-34, 144/2-9, 10, 32),analgue_layer,GColorWhite, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentRight);
  text_layer_set_text(text_3, TEXT_3);

  text_6 = macro_text_layer_create(GRect(144/2-6, 144-39, 10, 32),analgue_layer,GColorWhite, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentRight);
  text_layer_set_text(text_6, TEXT_6);

  text_9 = macro_text_layer_create(GRect(21, 144/2-9, 10, 32),analgue_layer,GColorWhite, GColorClear, fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentRight);
  text_layer_set_text(text_9, TEXT_9);

  alarm_layer = bitmap_layer_create(GRect(2, 129, 12, 12));
  layer_add_child(analgue_layer, bitmap_layer_get_layer(alarm_layer));
  bitmap_layer_set_bitmap(alarm_layer, alarm_bitmap);
  layer_set_hidden(bitmap_layer_get_layer(alarm_layer), true);

  weekend_layer = bitmap_layer_create(GRect(2, 129, 12, 12));
  layer_add_child(analgue_layer, bitmap_layer_get_layer(weekend_layer));
  weekend_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEEKEND_ICON);
  bitmap_layer_set_bitmap(weekend_layer, weekend_bitmap);
  layer_set_hidden(bitmap_layer_get_layer(weekend_layer), true);

  powernap_layer = macro_text_layer_create(GRect(144-21, 144-28 ,20, 31), analgue_layer, GColorWhite, GColorBlack, fonts_get_system_font(FONT_KEY_GOTHIC_24), GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(powernap_layer), true);
  // init hands
  // init hand paths
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  hands_layer = layer_create(GRect(0,0,144,144));

  GPoint center = grect_center_point(&GRect(0,0,144,144));
  gpath_move_to(minute_arrow, center);
  gpath_move_to(hour_arrow, center);

  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(analgue_layer, hands_layer);
}

/*
 * Set the power nap text for the analogue display
 */
void analogue_powernap_text(char *text) {
  strncpy(powernap_text, text, sizeof(powernap_text));
  if (strlen(powernap_text) > 0) {
    layer_set_hidden(text_layer_get_layer(powernap_layer), false);
    text_layer_set_text(powernap_layer, powernap_text);
  } else {
    layer_set_hidden(text_layer_get_layer(powernap_layer), true);
  }
}

/*
 * Set the weekend indicator on the analogue display
 */
void analogue_weekend(bool weekend) {
  layer_set_hidden(bitmap_layer_get_layer(weekend_layer), !weekend);
}

/*
 * Triggered when the sliding in/out of the analogue face completes
 */
static void animation_stopped(Animation *animation, bool finished, void *data) {
  if (is_visible) {
    bed_visible(false);
  }
  animation_unschedule((Animation*) analogue_animation);
  animation_destroy((Animation*) analogue_animation);
}

/*
 * Make the analogue watchface visible or invisible
 */
void analogue_visible(bool visible) {
  if (visible && !is_visible) {
    GRect start = ANALOGUE_START;
    GRect finish = ANALOGUE_FINISH;
    analogue_animation = property_animation_create_layer_frame(analgue_layer, &start, &finish);
    animation_set_duration((Animation*) analogue_animation, 1000);
    animation_set_handlers((Animation*) analogue_animation, (AnimationHandlers ) { .stopped = (AnimationStoppedHandler) animation_stopped, }, NULL /* callback data */);
    animation_schedule((Animation*) analogue_animation);
  } else if (!visible && is_visible) {
    GRect start = ANALOGUE_FINISH;
    GRect finish = ANALOGUE_START;
    analogue_animation = property_animation_create_layer_frame(analgue_layer, &start, &finish);
    animation_set_duration((Animation*) analogue_animation, 1000);
    animation_set_handlers((Animation*) analogue_animation, (AnimationHandlers ) { .stopped = (AnimationStoppedHandler) animation_stopped, }, NULL /* callback data */);
    animation_schedule((Animation*) analogue_animation);
    bed_visible(true);
  }
  is_visible = visible;
}

/*
 * Unload the analogue watchface
 */
void analogue_window_unload() {
  gpath_destroy(minute_arrow);
  gpath_destroy(hour_arrow);
  bitmap_layer_destroy(alarm_layer);
  bitmap_layer_destroy(weekend_layer);
  gbitmap_destroy(weekend_bitmap);
  gbitmap_destroy(logo_bitmap);
  bitmap_layer_destroy(logo_layer);
  text_layer_destroy(powernap_layer);
  text_layer_destroy(text_12);
  text_layer_destroy(text_3);
  text_layer_destroy(text_6);
  text_layer_destroy(text_9);
  layer_destroy(hands_layer);
  layer_destroy(analgue_layer);
}
