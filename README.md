# swamp-gas
Design for an illuminated saucer rocket

/3d-objects              STL files for 3D-printed parts
                         F3D and STEP files for complete rocket
/lighting-controller
  /arduino/swamp-gas     Arduino code for the lighting program
  /board                 design files for carrier PCB
    /images              images of the layout and manufactured board

Parts list:
  29mm motor tube, 100mm long
  2.6" body tube, 100mm long
  29mm motor retainer
  5 fins cut from 1/8" basswood, per template
  2-meter Adafruit NeoPixel strand, 60 pixels/meter
  Adafruit Trinket M0
  Custom carrier PCB
  4 green LEDs mounted in top of body tube to illuminate the dome
    (wired in parallel, with a current limiting resistor if needed)
  400mAh LiPo battery (zip tied to one of the fins)
  26 ga stranded wire
  2-pin JST connectors for battery
  3-pin Molex connectors for connection to NeoPixel strand
  2-pin Molex connectors for connection to LEDs in dome
  Power switch (optional)

LED wiring is left as an exercise for the reader and will depend on
the specific LEDs and battery you use.

In the first flight, the saucer flipped nose-down at apogee and fell
in a stable nose-down orientation, so no parachute should be needed.
The nose cone can be permanently mounted in the motor tube with CA.
Make sure the ejection sharge is removed from the motor before flight.

First flight was at the St. Louis Rocketry Association Spooktacular
Night Launch on October 30, 2021.

https://youtu.be/KtkhaTuiWWw

Hardware and software designs in this repository are distributed under
GNU GPLv3. (See COPYING for full license.)
