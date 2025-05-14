# Rhythm Sequencer 
<img src="https://github.com/user-attachments/assets/afbf6676-047c-4472-9f29-62564c1d1896" width=40% height=40%>

## Description  
This module is a 7-channel gate sequencer in a small form factor with an intuitive interface. It takes in a 5V clock signal,
and outputs gates on its channels according to the programmed sequence. Behind the scenes, this module is moving through an 
array of boolean values that represent whether a gate should be played on that step or not. The current step + next 7 steps 
are displayed on the LED matrix.

### Controls
The "Mode" switch changes whether you are in Programming mode or Play mode. In Programming mode, the "Step" button moves to 
the next step in the sequence, while the "Track"  button changes which channel you are editing. The "Add" button adds or 
removes a beat from the current step/track location in the array. Pressing and holding the "Add" button erases the entire array. 
Both the "Step" and "Track" controls wrap around when you reach the limits of the array. The "Store" button saves the entire 
array to EEPROM. If you are editing the middle of the array and want to go back to the beginning, flipping the "Mode" switch to
Play mode and back will reset your position to the beginning of the array.

In Play mode, you can use either the clock input or the "Step" button to move to the next step in the array. The "Store" button 
loads the saved array from EEPROM. Placing a gate in the "RST" track resets the array back to the beginning when that step is 
reached. The outputs are ANDed with the clock input, which means that your clock duty cycle will also be the output duty cycle.

## Module information
### Features
- 1 clock input, passed through to a clock output
- 7x 5V gate output channels
- Intuitive interface & control scheme
- Load/store 1 pattern to/from memory (may add ability to use multiple slots later)
- 32-step track length (easy to make it longer by changing my code)

### Power Requirements
500mA +5V

### Files
- Arduino .ino file
- KiCad .sch schematic file
- Circuit diagram .pdf file
- *I am not including PCB files at this time, as my system is 2U and would require extensive rework to fit in a traditional Eurorack
  system. You will have to make your own for now, but I tried to make it easier for you by including the .sch file.
