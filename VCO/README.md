# VCO
<img src="https://github.com/user-attachments/assets/6cabfaf6-2804-4c6e-b5ab-614565d91d1f" width=20% height=20%>

## Description
This is my take on Moritz Klein's VCO design. Some of the changes I made include some tweaked component values, 2n390x transistors,
and an op-amp based schmitt trigger to replace the 40106 chip. The latter change saves you from buying another random chip, which was 
mostly unused anyway. The op-amp schmitt trigger works exactly the same as far as I can tell, and should theoretically have better
performance. The 40106 can only source/sink 1mA, limiting how fast it can fill the main capacitor. This can affect the tuning at higher
frequencies. The TL074, in contrast, can output 20mA of current, leading to faster rise times.

## Module Information
### Features
- Sawtooth, square, and triangle wave outputs
- Tuning control
- Temperature compensation for greater tuning stability

### Power Requirements
10mA +12V, 10mA -12V

### Files
- KiCad .sch schematic file
- Circuit diagram .pdf file
- *I am not including PCB files at this time, as my system is 2U and would require extensive rework to fit in a traditional Eurorack
  system. You will have to make your own for now, but I tried to make it easier for you by including the .sch file.

