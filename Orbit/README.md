# Orbit LFO  
<img src="https://github.com/user-attachments/assets/19bf3e8d-ba15-47b2-9ec3-0fdd290280b1" width=20% height=20%>

## Description
This is Orbit, an STM32-based Low Frequency Oscillator (LFO) that I designed for my custom modular 
synthesizer. It features controls for frequency, phase, and amplitude, as well as seven different selectable 
waveforms.

## Module Information
### Features
The seven available waveforms are:
- sine
- square
- ramp
- sawtooth
- triangle
- random ramp
- random step

The waveform is selected via the "Wave" knob on the front, and the current waveform is indicated by an LED.
You can control the frequency of each waveform via the "Period" knob, through the Tap Tempo button, or by syncing it to an 
external 5V digital signal through the "Sync" input. The phase offset of the wave relative to the sync signal can be controlled 
by the "Phase" knob, and the amplitude of the wave is controlled by the "Height" knob. The peak amplitude of each waveform is
quantized to a 12-TET V/oct standard, with a range of 5 octaves (0-5V). The module features one normal output and an inverted 
output that subtracts the generated waveform from 5V.

### Hardware
I developed this project on an STM32L432KC NUCLEO dev board, but the code should work on any STM32L4xxxx 
chip. If I get bored sometime I may try to make it work on an Arduino.
**IMPORTANT**: In order for the NUCLEO board to work properly, you will need to remove the SB15, SB16, and SB18 solder bridges
from the back of the board. This will disable the onboard LED, and disconnect PA5+PA6 from PB6+PB7, allowing them to be used
separately.

### Power Requirements
~30mA +12V, ~5mA -12V, ~5mA 5V

### Software:
The project is coded completely in bare-metal C, utilizing direct register manipulation rather than 
CubeIDE's HAL or LL functions. I tried to add comments to just about every line (mostly for my own sanity), 
so you should be able to reverse-engineer and/or modify my code if you would like.

### Files

- STM32 CubeIDE project folder, with all relevant files for programming the device
- KiCad .sch schematic file
- Circuit diagram .pdf file
- *I am not including PCB files at this time, as my system is 2U and would require extensive rework to fit in a traditional
  Eurorack system. You will have to make your own for now, but I tried to make it easier for you by including the .sch file.
