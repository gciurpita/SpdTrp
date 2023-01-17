### Loco Speed Trap

<img src=https://i5.walmartimages.com/asr/4b1a317d-14a7-4a60-bc2b-5391fe8575e4.7a4267827bfc80a2badfd102734ec542.jpeg height=300 align=right>

A locomotive speed trap reports the speed in scale miles per hour
with one decimal digit (e.g. 25.4).
A relatively inexpensive multifunction shield having
a four digit 7-segment display and electronics for driving it
can be stripped of unnecessary parts and mounted in the fascia.

A variety of optical sensors can be used,
preferably the reflective type mounted between track ties.
They need to be soldered to the multifunction shield
since the shield plug into the connectors on the Arduino.

A USB charger and cable can provide power.

The distance between sensors and scale as a number (e.g. 87 for HO)
can be stored in EEPROM to accommodate various installations.
The configuration can easily be changed via the serial monitor
of the Arduino IDE or any suitable serial program
connected via the Arduino USB interface.

The logic captures a timestamp at either end of the trap and
computes the speed after capturing the timestamp
when it exits the trap.
It ignores further sensor activity as cars repeatedly trip sensors
until neither sensor becomes active for several seconds.

The code is composed of three main files and corresponding .h files.
`pcRead.cpp` has three functions for reading and writing to the EEPROM:
`eeRead16()`, `eeWrite16()` and `eeDisp()`.
It also has `pcRead()` which monitors the serial interface to
report and modify parameters.

<!-- 
pcRead.cpp:eeRead16 (
pcRead.cpp:eeWrite16 (
pcRead.cpp:eeDisp (void)
pcRead.cpp:pcRead (void)

seg7disp.cpp:void output (
seg7disp.cpp:void isr (void)
seg7disp.cpp:void seg7disp (
seg7disp.cpp:void seg7off (void)
seg7disp.cpp:void seg7on (void)
seg7disp.cpp:void seg7init (void)
 -->

`seg7disp.cpp` has routines to use the 7-segment display.
The display is multiplexed
which means it only display one digit at a time and
therefore needs to rapidly sequence through each digit.
It uses an timer driven interrupt `isr()`
to get the next bitmap to display and calls `output()`
to shift bits out to two shift registers driving the 12 pins of the display.
`seg7off()` and `seg7on()` turn all the segments on or off.
`seg7disp()` translates the digits to be displayed to the 7-segment bitmaps
blanks out leading zeroes and
turns decimal point on in the second to last digit.
`seg7init()` configures the pins and interrupt.

<!--
SpdTrp.ino:const unsigned Nsensor = sizeof(PinSensor);
SpdTrp.ino:setConversion (void)
SpdTrp.ino:dispSpd (
SpdTrp.ino:trap (void)
SpdTrp.ino:loop (void)
SpdTrp.ino:setup (void)
 -->

`SpdTrp.ino` is the main file with the required .ino suffix
allowing the code to be compiled using the Arduino IDE.
`setConversion()` reads the distance and scale parameters from EEPROM and
computes the value used to translate the trap time to scale MPH.
`dispSpd()` translates the trap time in msec to MPH,
calls `seg7disp()` and prints it to the serial monitor.
`trap()` monitors the sensors,
calls `dispSpd()` when the loco exits the trap and
waits for the trains to pass when there is no more sensor activity.
`loops() calls `traps()` and `pcRead()`.
`setup()` performs initialization,
invoking `seg7init()`,
calls `setConersion()`,
configures the sensor pins and
flashes all segments of the display to show all segments work.

The code can be downloaded from
[https://github.com/gciurpita/SpdTrp](https://github.com/gciurpita/SpdTrp)

<p align=right>
Greg Ciurpita, Jan 2023
