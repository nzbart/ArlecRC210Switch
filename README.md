# Arlec RC210 AU/NZ 240V socket remote controlled power outlets

Sold at Bunnings in Australia and New Zealand as a three-pack (RC213) or one-pack (RC210):
* Three pack at Bunnings [New Zealand](https://www.bunnings.co.nz/arlec-white-remote-controlled-power-outlets-3-pack_p00327678) or [Australia](https://www.bunnings.com.au/arlec-remote-controlled-power-outlets-3-pack_p4331764).
* One pack at Bunnings [New Zealand](https://www.bunnings.co.nz/arlec-white-remote-controlled-power-outlet_p00327675) or [Australia](https://www.bunnings.com.au/arlec-remote-controlled-power-outlet_p4331763).
* Arlec appears to be a Bunnings brand, but the brand [has a website](https://www.arlec.com.au/).
* I haven't tried these, but they look suspiciously like a [similar product from Jaycar](https://www.jaycar.co.nz/remote-controlled-3-outlet-mains-controller/p/MS6147).

# Installation

Use this repository directly, or install via the platformio package:

* [ArlecRC210Switch](https://platformio.org/lib/show/5543/ArlecRC210Switch)

# Sample code

```c++
#include <Arduino.h>
#include <ArlecRC210Switch.h>

namespace {
    const auto rf433OutputPin = 10;
    ArlecRC210Switch arlecSwitch;
}

void setup() {
    arlecSwitch.begin(rf433OutputPin);
}

void loop() {
  // Switch numbers are from 0 to 7. 
  // Only 3, 5, 6, and 7 appeared to work for me.
  
  arlecSwitch.setArlecSwitchState(6, true);
  delay(5000);
  arlecSwitch.setArlecSwitchState(6, false);
  delay(5000);
}
```

# Problems with these switches

* There appears to be crossover between the switches, and they can sometimes switch on or off with the wrong address. This also happens with the provided remote control. The fix is to reset the switches by plugging them in and pressing the ☀ until the LED flashes quickly and then turns off. You can then reprogram the switches. 
* Since the switches are reprogrammable, if there's a power cut or someone turns off one of the sockets, you may be sending a signal to another socket when the first socket is turned on. This would set both sockets to the same code, so you'll be inadvertently controlling two sockets. You should code for this possibility, including:
    * Trying to avoid sending any signals for at least 40 seconds after power is restored from a power cut.
    * Trying to detect whether the application you're controlling is having an effect, for example if controlling a heater, is the room heating up after a reasonable time?

# The protocol

The chip in the remote doesn't have any markings at all.

There don't appear to be any individual serial numbers on the sockets. It appears that the remote conrol is pre-programmed with 4 different fixed codes, and the individual sockets can be paired with any of those codes by pressing the button on the remote when the socket is powered on.

My particular remote control has the following 4 codes (the 32 data bits + the 1st sync bit are shown) to turn the switches on, in addition to one that turns all switches on:

* A:   011101101101100000001111100111100
* B:   011101101101100000001101100111000
* C:   011101101101100000001011100110100
* D:   011101101101100000000111100101100
* All: 011101101101100000000100100101010

To turn the switches off, the 23rd (base 0) bit is set to 0 and the 31st bit is set to 1.

For example, to turn D on or off:

```
* On:  011101101101100000000111100101100
* Off: 011101101101100000000110100101110
                              ^       ^
```

A pattern emerges if you break the bit stream up:
```
A: 01110110110110000000 111 1 1001 111 0 0
B: 01110110110110000000 110 1 1001 110 0 0
C: 01110110110110000000 101 1 1001 101 0 0
D: 01110110110110000000 011 1 1001 011 0 0
*: 01110110110110000000 010 1 1001 010 0 0
```
The first part is identical for all. The next three bits appear to identify the switch. The following bit is on/off. The next block is always 1001. The block after that appears to be the switch identifier repeated. Then follows the inverse of the on/off switch bit, and another zero. 
