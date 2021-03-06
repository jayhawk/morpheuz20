Morpheuz Sleep Monitor with Smart Alarm
=======================================

Do you ever wonder how well you sleep? The Morpheuz Sleep Monitor uses the Pebble's built in accelerometer to monitor your night's sleep and provide a graph of how much you moved overnight.

Morpheuz is a watch app that monitors your movement and periodically transmits this information to your iPhone or Android phone, where the Javascript component within the Pebble app stores it. The Morpheuz configuration page in the Pebble app shows a graph, allows the setting of a smart alarm and resetting for the next night.

The "smart alarm" defines your earliest and latest wake up times. Morpheuz will do a 30 second watch vibration if you stir during that time period or, should you remain motionless, at the end of that time period.

No data is stored anywhere other than on the phone. Data is sent to the configuration page for graphing but this stores no information.

It's simple to install. There is no iOS or Android app to buy, nothing to sign up for and it's free.

Please visit http://www.morpheuz.net for further information.

Sleep well.

Version 2.4
-----------
* The alarm starts with 10 shorter vibes before escalating - some people found the alarm initially too strong.
* There is an optional analogue watchface.
* Analogue display shows; reset time, earliest wakeup time, latest wakeup time, progress, alarm active, weekend mode and powernap timer.
* Instead of the increasingly weird button combinations to access basic functions and the 'button help page' which appeared on a single press of any button I have introduced regular Pebble style menus. This makes Morpheuz seem more familiar, allows the pressing of a button to illuminate the backlight, and provides the means to introduce more features in the future.
* back button still needs a long press to leave (this is to prevent accidental closure of Morpheuz in your sleep). A short press does nothing.
* up button cancels the alarm if it is sounding or snoozed, otherwise does nothing
* down button snoozes the alarm if it is sounding, otherwise does nothing
* select brings up a menu with the following options:
1) Ignore (was a double click of select)
2) Reset (was a long press of up)
3) Power nap (was a long press of select)
4) Weekend (was a long press of down)
5) Resend (was a double press of back)
6) Inverse (was on the phone config page and could only be changed on a reset)
7) Analogue (was, briefly, on the phone config page and could only be changed on a reset)
9) Quit (still available as a long press on back)
* When the alarm is sounding then two additional menu items appear at the top of the list:
a) Snooze Alarm (the down button will also work)
b) Cancel Alarm (the up button will also work)
* Menu items which are toggles have a tick or cross against them.
* I have added the date across the top of the menu. A lot of the messages that were there to reassure that the correct button was pressed have been removed as they were overkill.

Version 2.3
-----------
* The chart displays are more modern looking (strong colours, shades & no drop shadows). I hope you like these.
* Morpheuz attempts to work out the start and end of your period of sleep - this is represented on the chart as two vertical solid orange lines. The first is set for the first 10 minute period where your movement is below the threshold set for awake. The last is set to either the actual alarm time or if the alarm didn't go off the last 10 minute period where your movement is below the threshold set for awake.
* The start and end of the sleep period is reported in a new statistics section called 'Sleep Analysis Data Point' - this is intended for entry directly into Apple's Healthkit.
* The trend line has been removed from the graph - it had little purpose anyway as it was always messed up by the waking time after the alarm.
* The drop downs for minutes (iOS only) has been changed to be in 5 minute rather than 15 minute intervals - you may now choose to wake at 7:10!
* The text boxes for hours and minutes (Android only) have been set to type number. This should hopefully give Android users a numeric keypad on entry.
* The email export attempts to close the morpheuz config window as leaving it open on ios8 caused the Pebble app to hang.
* Once you have slept and got some 'Sleep Analysis Data Point' data a button will appear below (iOS only). The purpose of this button will be revealed shortly, although you can probably guess.
* It's all been recompiled with Pebble SDK 2.6.1
* Morpheuz now has a background process. Since Pebble only runs on background process at a time, this ensures that you are reminded that something else is running. You have the choice to suspend it (answer yes) whilst Morpheuz is running or allow it to run (answer no). If you answer no the other activity monitoring app may close down Morpheuz and prevent the alarm and full overnight charting.

Version 2.1.0
----------------
* NEW Weekend mode - One button press prevents the smart alarm going
off and automatically re-enables it for the next night. Ideal for those
nights when you go to bed knowing you don't need to get up. Activated
by a long press on the down button either before or after the long
press on the reset button.
* Further battery optimisation - calculation for smart alarm threshold
was taking place outside of the smart alarm period. This was a waste.
* Interfacing of the smart alarm actual time is now done as part of the
assured delivery loop. This prevents the smart alarm actual time being
lost if the watch app is shut when comms is down.
* New menu icon - again. Not satisfied with the last one.
* Reset does a double buzz if the smart alarm is on and a single buzz
if it is off. As before a total failure gives an SOS buzz.
* Built with latest SDK
* Comms icon cannot be at odds with BT icon

Version 2.0.0
----------------
* New graphics; 
* SOS buzz when accelerometer is not working; 
* better battery life;

Version 1.9.0
-------------
* Improved alarm icons.
* Removed battery wasting zzz. If the accelerometer stops then a single attempt will be made to re-register with it and then a full screen message with clear advice will be displayed.
* Alarm snooze is now 9 minutes (standard on most alarm clocks I’m told - also the same as iOS) rather than the 5 I previously set.
* Fixed the 12/24 hour problem when set for 12 the time was displayed wrongly.
* Increased the time font size on the button hint screen - so as pressing a button in the middle of the night shows the time as well as giving a hint as to which buttons are which.
* Pressing any button again on the button hint screen makes it go away again.
* New button hint screen appears when a button is single pressed.
* Alarm is ‘progressive’ starting with a vibe, delay sequence and getting progressively more aggressive. The alarm repeats 3 times with a 1 minute delay between repeats.
* Alarm can be cancelled by a single press of the up button (matches the alarm app)
* Alarm can be snoozed by a single press of the down button (matches the alarm app)
* Alarm indicator in the top bar.
* Fixed a minor memory leak introduced in earlier releases.

Version 1.8.0
-------------
* Back button requires long press
* Clear onscreen notices
* Alarm can be cancelled by pressing any button

Version 1.7.0
-------------
* Runs without persistent connection to phone
* Data transfer when connected to phone
* Reset from watch
* Progress bar
* Recording indicator
* Flashing accelerometer indicator

Version 1.6.0
-------------
* Power nap
* Bluetooth and comms indication
* Hourly statistics

Version 1.5.0
-------------
* SDK2 BETA4
* Use Pebble accelerometer vibration indication - much simpler than my
original technique
* Add inverse display
* Improved algorithm for determining sleep quality

Version 1.4.0
-------------
Ensured that parseInt is always called with a radix of 10. Conversion in some browsers (iOS) makes 09 = 9 decimal. In others (Android) makes 09 = 0. 

Version 1.2.0
-------------
* Make config buttons work for Android
* Ensures that if the app hasn’t got focus then we don’t consider the
accelerometer data (probably the alarm is going off)
* Ensure it’s own alarm doesn’t produce a movement spike
* Plot the start and end of the smart alarm period, plus now the time
the smart alarm actually went off.
* Visual clues to tie the start and end times against the graph
* Reduced the emphasis of the trend line - which is interesting but not
*that* interesting.

Version 1.1.0
-------------
* Compiled for 2.0 beta3
* Improved graphing adding trend line
* CSV mailto: now vertical
* Minutes on CSV now minutes not months.
* Serious code tidyup on web hosted side

Version 1.0.0
---------------
* Better graphics
* Smart alarm times on watch screen
* Nicer fonts
* Now includes MIT Licence.

Version 0.8.0
-------------
* Copy option - copies as CSV data in a mailto (appears as an email document)

Version 0.7.0
-------------
* Battery meter on screen at all times
* Version shown on watch face and in config window
* Date (ISO format) shown on graph
* Resolution of graph now 1 point per 10 mins not 1 point per 15 mins.

Version 0.6.0
-------------
* Watchapp not watchface.
* Automatic self monitoring to reboot comms and accelerometer if problems occur
* Better graphing - gaps where no data found.
* Removed bug with resetting timer service repeatedly - could have resulted in poor battery life.

Version 0.5.0
-------------
Added Smart Alarm. Set a time period from the earliest time to the latest time you wish to be woken and Morpheuz will wake you during that period with 30 seconds of vibration. It will attempt to alarm when you are restless rather than in a deep sleep. Unless it has already done so it will alarm at the end of the period.

Added a time scale to the sleep activity graph.

Extended the monitoring timeframe to 9 hours.

PLEASE NOTE: This is beta software on a beta OS. Whilst I am confident Morpheuz will not attempt to alarm outside the period specified, there is a possibility that it may not alarm at all. Please ensure that you set a backup alarm. During testing I've had the watch face swap during the night and this disables monitoring and the alarm function.

Also note, the new timeframe is only saved when the "Reset and Go" button is pressed, not when "Done" is pressed.

The vibration can be cancelled by switching to another watch face.

Version 0.1.0
-------------
Original version.


