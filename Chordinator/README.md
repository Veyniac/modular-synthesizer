# Chordinator  
<img src="https://github.com/user-attachments/assets/3af476db-6143-4e2b-9fe9-a4880dcf00f1" width=40% height=40%>

## Description
The Chordinator is, in its simplest form, a 3-channel CV quantizer, with one of the inputs quantized to a user-selectable scale.
That on its own is pretty useful, but where the Chordinator really shines is when you use _fewer_ of its inputs. By plugging in
only a single CV source into the module, you can use it to control three different oscillators and make a chord. When only given
a single CV source, the module will use that CV value as the root note, and automatically calculate the CV values required to
produce the 3rd and 5th notes of the triad. The knob on the front controls the quality of the chord, and which input you use
changes the inversion of the triad.

The table below summarizes the different modes of the module:  
<img src="https://github.com/user-attachments/assets/462b81ea-bcb6-4c29-a0aa-948481f1331a" width=60% height=60%>

The custom scale that the "main" input is quantized to is selected and displayed via the 12 illuminated push buttons on the front
panel. You can use the buttons to create your own custom scale, or you can press and hold one of the buttons to change the key to
that note automatically.

The push button LEDs automatically show the chosen scale and which notes are being played. Notes that are currently being played 
are fully on, notes that are "allowed" are dimmed, and notes that are not allowed are fully off.

## Module Information
### Features:
- 3 inputs, 3 outputs
- 12 TET, V/oct tuning
- 0-5V input range
- 0-5V (5 octave) output range
- 1 input quantized to custom equal-temperament scale
- Chord quality control
- Chord inversion control
- Analog input overvoltage protection
- RC low-pass filters on inputs
- Digital input filtering

### Power Requirements: 
~60mA +5V

### Files
- Arduino .ino file
- KiCad .sch schematic file
- Circuit diagram .pdf file
- *I am not including PCB files at this time, as my system is 2U and would require extensive rework to fit in a traditional Eurorack
  system. You will have to make your own for now.

