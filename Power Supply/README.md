# Eurorack Power Suppy

## Description

This is a power supply designed for use with a modular Eurorack synthesizer system. It is designed
to deliver high power with low noise, while maintaining a commpact form factor.

WARNING: this is an advanced project. Do not attempt this without a basic knowledge of electronics
and through-hole soldering. I take no responsibility for my supply blowing up your expensive gear. 
I am doing this for fun and for free, and am liable to make mistakes. However, I have been using
this supply with my system for over a month now, and have had no issues so far.

Additional notes:
- The GATE pins on each header are connected together.
- The CV pins on each header are connected to ground.
If your modules need the above pins to talk to each other, this board probably isn't for you.

## Specs

- 9~36V input voltage range
- 65W total power*
- +5V 2.5A
- +12V 1.5A
- +12VA 1.5A
- -12V 1.5A

Measurements: 345.44 x 50.8 x 20 mm (13.6 x 2 x 0.8 in)

****Specs given are around 50% of what the supply is theoretically capable of. You may be able***
***to squeeze as much as 120W out of the DC-DC converters by sizing up the input fuse, but I***
***can't guarantee that the PCB will be able to handle that much current. Do so at your own risk.***

## Features

- Short circuit protection on output
- Overvoltage protection on input
- Reverse polarity protection on input
- Fused input
- Overload protection
- Over temperature protection
- Under voltage lockout
- Output voltage trim (±10%)
- Rail power indicator LEDs
- Voltage measurement test points
- Power on/off switch functionality
- Low noise (<50mVpp)

## Files

BOM: Eurorack_Power_Supply_BOM.csv  
PDF Schematic: Power_Supply_Schematic.pdf  
PDF Board top view: Power_Supply_PCB.pdf    
Kicad Schematic: Power_Adapter_2.0.kicad_sch  
Gerbers: Power_Supply_Gerbers.zip  

## Build Guide

1. Start by soldering your chosen input jack (J1) to the board with however long of wires you think you need.
Note that the positive lug of the jack should go to the square hole of J1, while ground should go to the
circular hole.

2. Next, add the fuse & holder (F1) and the reverse polarity protection diode (D1). Now, apply power to the
board. If the diode gets hot or the fuse explodes, either you messed up somewhere or your supply isn't the
polarity you think it is. If nothing bad happens, solder C1, C2, and C3 before moving to the next step.

3. Now we are going to move to the other end of the board and add the power indication LEDs. Solder on D1, D2,
D3, D4, R1, R2, R3, and R4. Optionally, add TP1, TP2, TP3, TP4, TP5, TP6, TP7, and TP8 for easily measuring the
voltage on each rail.

4. With our LEDs added, we can start adding the DC-DC converters. Start with U1. You may need to turn up the
temperature of your soldering iron, as the converters have thick pins and large heatsinking enclosures. After U1
is added, plug in the board again. D2 should light up, and you should be able to measure 5V on TP2.

5. OPTIONAL: to ajust the output of the DC-DC converter to your liking, first measure what the output is using
the test points. It should be within 5% of the voltage marked next to the test point. If that isn't good enough for
you, add the corresponding trim potentiometer (RV1, RV2, RV3, or RV4). If you want to increase the output voltage,
use a blob of solder to short the even-numbered solder jumper (JP2, JP4, JP6, or JP8). If you want to lower the
output voltage, short the odd-numbered solder jumper (JP1, JP3, JP5, or JP7). Turn the potentiometer until the
output voltage is where you want it.

6. Solder the two capacitors to the right of the DC-DC converter (e.g. C4 and C8).

7. Repeat steps 4-6 for U2, U3, and U4, making sure to power on the board regularly to check for issues. Better
to find out something is messed up before adding all your expensive DC-DC converters. Once those are on, they are 
not coming off. D3, D4, and D5 should light up in that order as you add the DC-DC converters, with about the same
intensity. If one of them doesn't come on, check the voltage on the test point before panicking. If that's fine,
you probably just have a bad or backwards LED.

8. Once all the DC-DC converters are added, you are ready to solder in all of the headers! MAKE ABSOLUTELY SURE
that each header is facing the correct way when you solder it in according to the silkscreen. If you get it wrong,
if will be almost impossible to remove the header and put it back in the right way (ask me how I know).

9. OPTIONAL: if you want to be able to turn the board off without unplugging it, wire a SPST switch to SW1.
When the two pads are shorted, the DC-DC converters all switch off!

