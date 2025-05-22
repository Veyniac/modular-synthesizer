# Low-Pass Gate  
<img src="https://github.com/user-attachments/assets/9b39ed32-e588-4f25-a80b-ad864ac3005b" width=35% height=35%>

## Description
This is my take on a low-pass gate design. It is quite a bit simpler than the classic Buchla-style design,
but it is a lot easier to understand, build, and use. The main difference is the lack of resonance, which I may end up
going back and fixing later.

The overall circuit is simply a voltage-controlled low-pass filter followed by a voltage controlled amplifier. The
voltage control is accomplished via vactrols, which I made myself out of some LEDs and LDRs I had lying around. You may
need to modify some resistor values for whatever vactrols/LDRs you end up using. I used GL5528 LDRs, which have a "light"
resistance of ~2kOhm and a "dark" resistance of >1MOhm.

### Controls
The module has 3 CV inputs, 1 signal input, and 1 signal output. One CV input controls the VCA response, one controls the
VCF response, and one sends the same signal to both. The inputs are in an OR configuration, which means you can send a CV
signal to the "dual" input and the other inputs at the same time without frying anything. Whichever input is stronger
at the time will control the VCA/VCF.

The module features controls for both the strength of the VCA/VCF response and their "decay time". I designed this module
to mainly be triggered by 5V gates, so I added a simple decay envelope onto the CV inputs. This allows you to get piano-like 
notes from the module without an envelope generator.

The vactrols' response is displayed on the two front panel LEDs, which are in parallel with the vactrol LEDs. This allows
you to easily see what the CV signals are doing, and is a feature I don't see on many commercial LPGs. As a bonus, it makes
the module look cooler :)

## Module Information
### Features
- Individual or simultaneous control of the VCA and VCF
- Built-in decay envelope for CV inputs
- Filter and VCA strength control
- Vactrol response display LEDs

### Power Requirements
~20mA +12V, ~20mA -12V

### Files
- KiCad .sch schematic file
- Circuit diagram .pdf file
- *I am not including PCB files at this time, as my system is 2U and would require extensive rework to fit in a traditional Eurorack
  system. You will have to make your own for now, but I tried to make it easier for you by including the .sch file.
