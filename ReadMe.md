<table align=right> <tr><td> Greg Ciurpita, Jan 27 2023 </table>

## Loco Speed Trap

<img src=https://i5.walmartimages.com/asr/4b1a317d-14a7-4a60-bc2b-5391fe8575e4.7a4267827bfc80a2badfd102734ec542.jpeg height=300 align=right>

A locomotive speed trap reports the speed in scale miles per hour
with one decimal digit (e.g. 254 is 25.4 scale mph).
It uses a relatively inexpensive multi-function shield having
a four digit 7-segment display.
A variety of optical sensors can be used,
preferably the reflective type mounted between track ties.

The distance between sensors and scale as a number (e.g. 87 for HO)
will need to be programmed in EEPROM to accommodate various installations
which can easily be edited using the serial monitor
of the Arduino IDE used to program the Arduino.
A USB-B cable connects the Arduino to PC.

The logic captures a timestamp at either end of the trap and
computes the speed after capturing the timestamp
when it exits the trap.
It ignores further sensor activity as cars repeatedly trip sensors
until neither sensor becomes active for several seconds.

It displays a sequence of moving decimal points
to indicate that it is timing the train
while waiting for the train trip the second sensor.
It times out after 30 seconds.

The unit is mounted upside down with the USB connector facing down
to make it convenient to attach a USB cable.
This means the decimal points on the display
are at the top of the display and not usable.

<img src=Images/spdTrap.jpg width=100%>

### Assembly
The speed trap is composed of
<ul>
  <li> Arduino Uno
  <li> <a href=https://www.mpja.com/download/hackatronics-arduino-multi-function-shield.pdf>Multi-function shield</a>
stripped of unnecessary protruding parts.
  <li> 2 IR sensors
  <li> 2 3-pin headers (optional)
  <li> 2 3-pin plugs (optional)
  <li> USB-B cable
  <li> USB wall charger
</ul>

Like any Arduino shield, the multi-function shield
has pins that allow it to plug directly into a Arduino UNO.
It has a four digit 7-segment display and shift register chips
that provide an interface with a minimum of pins.
It is designed for experimentation and
contains a variety of components (e.g. sounder, pot, connectors)
Once the protruding parts are unsoldered and removed from the board
that unit can be fitted into a fascia opening to view the display.

Several pins on the shield need to be trimmed to avoid touching
the USB connector and electrical tape should be placed over
the connector to further prevent contact. 

Each IR sensor will require connections to
power, ground and an input pin.
There are a group of four 3-pin headers that provide
5V, ground and access to pins 5, 6, 9 and A5
<a href=https://3.bp.blogspot.com/-o_0BOfE9jzo/XDphpHo6gWI/AAAAAAAABhs/UxtNHctqj0c8qyl8ywKdWVWMJSgY16wwQCLcBGAs/s1600/Schematic.png> (see schematic).</a>
It would be convenient to solder 3-pin servo female connectors
(Gnd, 5V, out) to each IR sensor
so that it can be directly connected to the shield header during testing
and later to an extension when mounting on the layout

The Arduino can be downloaded from github (see below) and
programmed using the Arduino Integrated Development Environment
<a href=https://docs.arduino.cc/software/ide-v1/tutorials/Windows>
(download)</a>
with just a USB-B cable connected between a PC and Arduino.

<ul>
 <li> plug the shield plugged into the Arduino,
 <li> plug the IR sensors onto the shield headers 5 & 6,
 <li> plug the USB cable into the shield and PC
 <li> open the IDE, open the downloaded sketch and program the Arduino
</ul>

###  Configure & Testing
The speed trap needs to be configured for the distance between
the sensors and the scale.
These values are stored in EEPROM and only need to be done once and
can be changed at any time by plugging the USB cable into a laptop
and entering commands using the IDE serial monitor.

The distance in inches is specified with the number followed by 'D'.
"125D" would set the distance to 12.5 inches.
Similarly, "87S" would set the scale for 1:87 or HO.
Both 'd' and 's' can be entered to display the corresponding values.
A '?' displays the short list of available commands.

Because the USB connector is inconveniently located at the top of the shield,
the digits are displayed upside down so that the unit
can be mounted upside down with the USB connector on the bottom.
Unfortunately, this means the decimal-point isn't used and 
it must be understood that the last digit is tenths of a MPH.

Once configured, the speed trap is running
<ul>
 <li> pass something across one of the sensors 
 <li> the decimal-point for each digit should be moving across the display
 <li> pass something across the other sensor and the speed should be displayed
 <li> after a few seconds the display clears going dark.
</ul>

### Installation

The unit can be mounted behind the fascia with a cutout for the display.
the IR detectors need to be mounted facing upward between the ties
at the configured distance.
The USB cable is plugged into a USB wall charger for power
but can be plugged into a laptop to update the code.

Unnecessary components on the multi-function shield will
get in the way of mounting the Arduino behind the fascia of a layout.
Headers, sounder, pot switches and larger electrolytic capacitor
can be unsoldered and removed.
Three conductor wires for the IR sensors
can be soldered to the vias
where the 3-pin plugs for pins 5 and 9 were removed
through the back of the board to not interfere with the fascia mounting.

The display will flash on power up.
Triggering either detector causes the decimal-point sweep across the display
until the other detector is triggered and the speed reported.
The display goes dark after a few seconds.

### Description of Code
The code is available for viewing or downloaded at
[https://github.com/gciurpita/SpdTrp](https://github.com/gciurpita/SpdTrp)
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
calls `setConversion()`,
configures the sensor pins and
flashes all segments of the display to show all segments work.

Of course you are encouraged to customize the code to your liking.

