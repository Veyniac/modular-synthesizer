#include <Wire.h>

uint16_t key = 0b1010110101010000; //this pattern dictates the number of semitones between notes in a key, MSB first

//translates button scan results [output][input] to button numbers. 255 is an invalid signal / means no button is pressed
uint8_t buttonMap[4][4] = {{255,6,3,0},{9,255,4,1},{10,7,255,2},{11,8,5,255}};

uint8_t buttonPressed = 255;
uint8_t lastButtonPressed = 255;
long buttonPressedStart = 0;
long buttonPressedTime = 0;
uint16_t allowedNotes = 0xFFF0; //bits 15-4 are the allowed notes in a 12-note scale (1=allowed, 0=not allowed), bits 3-0 not used
uint16_t displayedNotes = 0x0000; //which notes are currently being played. bits 0-3 not used
uint16_t PWM = 0xFFFF; //modulates brightness of LEDs
uint8_t PWM_ctr = 0; //used to determine duty cycle
uint16_t display = 0; //bits 15-4 are the button LEDs, bits 3-0 are the potentiometer leds
uint8_t cableConnections = 0; //bitfield keeping track of which cables are plugged in
uint8_t chordQualityDisplay = 0;
uint8_t numNotes = 12; //how many notes (out of 12) are currently allowed per octave
uint8_t lastNumNotes = 0;
int8_t hysteresisFactor = 4; //threshold for analog values
int8_t strikeFactor = 2; //SF+1 = number of times in a row an input must be above/below the threshold to change the output
bool updateScaleArrayFlag = 1; //does the scale array need to be updated
uint8_t scaleArray[61]; //holds the allowed note values

#define dataPin 2 //SPI connections for shift registers
#define clockPin 3
#define latchPin 4

#define potentiometerInput A0 //analog inputs
#define input1 A1
#define input2 A2
#define input3 A3

int16_t input1Value = 0;
int16_t input2Value = 0;
int16_t input3Value = 0;
int16_t potentiometerInputValue = 0;
int8_t input1Strike = 0;
int8_t input2Strike = 0;
int8_t input3Strike = 0;
int8_t potentiometerInputStrike = 0;
int16_t lastInput1Value = 0;
int16_t lastInput2Value = 0;
int16_t lastInput3Value = 0;
int16_t lastPotentiometerInputValue = 0;

uint16_t output1Value = 0;
uint16_t output2Value = 0;
uint16_t output3Value = 0;

int8_t rootNote = 0; 
int8_t thirdNote = 0;
int8_t fifthNote = 0;
int8_t thirdAdjust = 0;
int8_t fifthAdjust = 0;

uint8_t message[8]; //holds I2C data to be sent

void setup() {
  Wire.begin();
  Wire.setClock(100000);
  DDRD |= 0b11111100; //pin modes
  DDRB = 0;
  PORTB = 0;
}

void loop() {
  //button control & logic
  buttonPressed = scanButtons(); //checks if a button is pressed
  if(buttonPressed!=255 && lastButtonPressed==255){ //button was just pressed
    buttonPressedStart = millis();
  } else if(buttonPressed==255 && lastButtonPressed!=255){ //button was just released
    buttonPressedTime = millis()-buttonPressedStart;
    if(buttonPressedTime<1500){
      //if the button was held for less than 1.5 seconds, toggle the allowed status of the associated note
      allowedNotes ^= 0x8000>>(lastButtonPressed); 
    }
  } else if(buttonPressed!=255 && (millis()-buttonPressedStart)>1500){ 
    //if a button has been held for more than 1.5 seconds, change allowed notes to the key of that note
    allowedNotes = ((key>>buttonPressed) | (key<<(12-buttonPressed))) & 0xFFF0;
    updateScaleArrayFlag = 1; //array of allowed note values needs to be changed
  }
  lastButtonPressed = buttonPressed; //keep track of the last result of scanButtons()

  //calculate the current number of allowed notes (per octave)
  lastNumNotes = numNotes;
  numNotes = binarySumOfOnes(allowedNotes);
  if(numNotes==0) { //reset allowed notes if no notes are allowed
    allowedNotes = 0xFFF0;
    numNotes=12;
  }
  if(numNotes!=lastNumNotes) updateScaleArrayFlag = 1; //if number of allowed notes has changed, the scale array needs to be updated

  //calculate a new array of allowed notes only if necessary
  if(updateScaleArrayFlag) makeScaleArray();

  //see which cables are connected
  cableConnections = testCableConnections();

  //input reading & filtering logic: prevents random noise from causing an erroneous note change through a hysteresis threshold and a "strike" system
  input1Value = analogRead(input1);
  if(input1Value>lastInput1Value+hysteresisFactor || input1Value<lastInput1Value-hysteresisFactor){ //is the signal past the hysteresis threshold?
    input1Strike++; //how many times has this occurred?
    if(input1Strike==strikeFactor){ //after 3 consecutive strikes, change the note
      input1Strike=0;
      lastInput1Value = input1Value;
    } else input1Value = lastInput1Value;
  } else {
    input1Value = lastInput1Value;
    input1Strike = 0;
  } 

  input2Value = analogRead(input2);
  if(input2Value>lastInput2Value+hysteresisFactor || input2Value<lastInput2Value-hysteresisFactor){
    input2Strike++;
    if(input2Strike==strikeFactor){
      input2Strike=0;
      lastInput2Value = input2Value;
    } else input2Value = lastInput2Value;
  } else {
    input2Value = lastInput2Value;
    input2Strike = 0;
  }

  input3Value = analogRead(input3);
  if(input3Value>lastInput3Value+hysteresisFactor || input3Value<lastInput3Value-hysteresisFactor){
    input3Strike++;
    if(input3Strike==strikeFactor){
      input3Strike=0;
      lastInput3Value = input3Value;
    } else input3Value = lastInput3Value;
  } else {
    input3Value = lastInput3Value;
    input3Strike = 0;
  }

  potentiometerInputValue = analogRead(potentiometerInput);
  if(potentiometerInputValue>lastPotentiometerInputValue+hysteresisFactor || potentiometerInputValue<lastPotentiometerInputValue-hysteresisFactor){
    potentiometerInputStrike++;
    if(potentiometerInputStrike==strikeFactor){
      potentiometerInputStrike=0;
      lastPotentiometerInputValue = potentiometerInputValue;
    } else potentiometerInputValue = lastPotentiometerInputValue;
  } else {
    potentiometerInputValue = lastPotentiometerInputValue;
    potentiometerInputStrike = 0;
  }


  //chord quality logic
  switch(map(potentiometerInputValue,0,1024,0,4)){
    case 0: //major
      thirdAdjust = 0;
      fifthAdjust = 0;
      chordQualityDisplay = 0b1000;
    break;

    case 1: //minor
      thirdAdjust = -1; //1 semitone down
      fifthAdjust = 0;
      chordQualityDisplay = 0b0100;
    break;

    case 2: //diminished
      thirdAdjust = -1; //1 semitone down
      fifthAdjust = -1; //1 semitone down
      chordQualityDisplay = 0b0010;
    break;

    case 3: //augmented
      thirdAdjust = 0;
      fifthAdjust = 1; //1 semitone up
      chordQualityDisplay = 0b0001;
    break;
  }


  //IO function logic
  switch(cableConnections){
    case 0b000: //nothing plugged in
      rootNote = 0;
      thirdNote = 0;
      fifthNote = 0;  
    break;
    
    case 0b001: //1 cable plugged in to input 1 (root)
      rootNote = quantizeFunction(input1Value); //quantize root note
      thirdNote = rootNote+4+thirdAdjust; //major 3rd is 4 semitones up
      fifthNote = rootNote+7+fifthAdjust; //perfect 5th is 7 semitones up
    break;

    case 0b010: //1 cable plugged in to input 2 (first inversion)
      rootNote = quantizeFunction(input2Value)+12; //quantize root note and add an octave
      thirdNote = rootNote-8+thirdAdjust; //major 3rd is 8 semitones down
      fifthNote = rootNote-5+fifthAdjust; //perfect 5th is 5 semitones down  
    break;

    case 0b100: //1 cable plugged in to input 3 (second inversion)
      rootNote = quantizeFunction(input3Value)+12; //quantize root note and add an octave
      thirdNote = rootNote+4+thirdAdjust; //major 3rd is 4 semitones up
      fifthNote = rootNote-5+fifthAdjust; //perfect 5th is 5 semitones down   
    break;

    case 0b011: //cables plugged in to inputs 1 and 2
      rootNote = quantizeFunction(input1Value); //quantize root note
      thirdNote = map(input2Value,0,1024,0,61); //quantize input 2
      fifthNote = rootNote+7+fifthAdjust; //perfect 5th is 7 semitones up    
    break;

    case 0b101: //cables plugged in to inputs 1 and 3
      rootNote = quantizeFunction(input1Value); //quantize root note
      thirdNote = rootNote+4+thirdAdjust; //major 3rd is 4 semitones up
      fifthNote = map(input3Value,0,1024,0,61); //quantize input 3     
    break;

    case 0b110: //cables plugged in to inputs 2 and 3
      rootNote = quantizeFunction(potentiometerInputValue); //quantize root note
      thirdNote = map(input2Value,0,1024,0,61); //quantize input 2
      fifthNote = map(input3Value,0,1024,0,61); //quantize input 3    
    break;

    case 0b111: //cables plugged in to all inputs (3-input quantize mode)
      rootNote = quantizeFunction(input1Value);
      thirdNote = map(input2Value,0,1024,0,61);
      fifthNote = map(input3Value,0,1024,0,61);    
    break;
  }

  //limit to max voltages
  if(rootNote>60) rootNote=60;
  else if(rootNote<0) rootNote=0;
  if(thirdNote>60) thirdNote=60;
  else if(thirdNote<0) thirdNote=0;
  if(fifthNote>60) fifthNote=60;
  else if(fifthNote<0) fifthNote=0;

  //convert notes to voltages
  output1Value = map(rootNote,0,61,0,4096);
  output2Value = map(thirdNote,0,61,0,4096);
  output3Value = map(fifthNote,0,61,0,4096);

  if(cableConnections==0) displayedNotes = 0; //no notes displayed if nothing is plugged in, for ease of scale programming
  else displayedNotes = (0x8000>>(rootNote%12) | 0x8000>>(thirdNote%12) | 0x8000>>(fifthNote%12)) ; //display played notes on LED keyboard

  //controls the allowed-but-not-played LED brightness
  PWM_ctr++;
  if(PWM_ctr%4==0) PWM = 0xFFFF;
  else PWM = 0;

  //constructs the data to be sent to the shift registers
  //allowed notes have a 25% duty cycle, played notes are on, disallowed notes are off
  display = (allowedNotes & (PWM & 0xFFF0)) | displayedNotes | chordQualityDisplay; 
  updateDisplay(display);
  
  //binary data sent to MCP4728
  message[0] = output1Value>>8;
  message[1] = output1Value;
  message[2] = output2Value>>8;
  message[3] = output2Value;
  message[4] = output3Value>>8;
  message[5] = output3Value;
  message[6] = 0;
  message[7] = 0;

  //update DAC values
  Wire.beginTransmission(0x60); //device address
  Wire.write(0b01010000); //device mode (sequential write)
  Wire.write(message,8); //writes the contents of message to the device registers, updating the various outputs
  Wire.endTransmission();
}

//scans the button matrix and returns which button is pressed (or 255 if none is)
uint8_t scanButtons(){
  uint16_t pressedButton = 255;
  for(int i=0;i<4;i++){
    DDRB &= ~0x0F; //set all charlieplexing pins to inputs
    DDRB |= 0x01<<i; //set pin i to output
    PORTB |= 0x01<<i; //set output pin high
    for(int j=0;j<4;j++){ //scan input pins sequentially
      if(j==i) continue; //skip output pin
      else if(PINB>>j & 0x01){ //read PINB shift to input pin j and check if true
        pressedButton = buttonMap[i][j]; //map signal to which button is pressed
        PORTB &= ~(0x01<<i); //turn off output pin
        return pressedButton;
      }
    }
    PORTB &= ~(0x01<<i); //turn off output pin
  }
  return pressedButton; //no buttons pressed
}

//fills the scale array with allowed note values
void makeScaleArray(){
  for(int i=0,pointer=0;i<12;i++){ //iterates through the bits of allowedNotes
    if((allowedNotes<<i) & 0x8000){ 
      //if bit is 1 (indicating the note is allowed), add that note and its octaves to the scale array
      scaleArray[pointer] = i;
      scaleArray[pointer+numNotes] = i+12;
      scaleArray[pointer+numNotes*2] = i+24;
      scaleArray[pointer+numNotes*3] = i+36;
      scaleArray[pointer+numNotes*4] = i+48;
      if(i==0) scaleArray[pointer+numNotes*5] = 60;
      pointer++;
    }
  }
}

//quantizes an input to the nearest allowed note by mapping it to its place in the scale array
uint8_t quantizeFunction(uint16_t input){
  uint8_t relativeNote = map(input,0,1024,0,(numNotes*5)+1);
  uint8_t note = scaleArray[relativeNote];
  return note;
}

//sends a high-low-high signal to the switched pin on each jack
//if the input signal matches this pattern, the jack has no cable plugged into it
uint8_t testCableConnections(void){
  uint8_t connectedCables = 0b111;

  //test if input 1 connected
  PORTD |= 0b1<<5; //send HIGH signal to tip switch
  delayMicroseconds(150); //small delay after pin set to account for capacitance
  if((PINC>>1) & 0b1){ //continue if input 1 reads HIGH
    PORTD &= ~(0b1<<5); //send LOW signal to tip switch
    delayMicroseconds(150); //small delay after pin set to account for capacitance
    if(!((PINC>>1) & 0b1)){ //continue if input 1 reads LOW
      PORTD |= 0b1<<5; //send HIGH signal to tip switch
      delayMicroseconds(150); //small delay after pin set to account for capacitance
      if((PINC>>1) & 0b1) connectedCables &= ~0b1; //if input 1 reads HIGH, there is no cable in the jack
    } 
  }
  PORTD &= ~(0b1<<5); //turn off signal to tip switch

  //test if input 2 connected
  PORTD |= 0b1<<6;
  delayMicroseconds(150);
  if((PINC>>2) & 0b1){
    PORTD &= ~(0b1<<6);
    delayMicroseconds(150);
    if(!((PINC>>2) & 0b1)){
      PORTD |= 0b1<<6;
      delayMicroseconds(150);
      if((PINC>>2) & 0b1) connectedCables &= ~0b10;
    } 
  }
  PORTD &= ~(0b1<<6);

  //test if input 3 connected
  PORTD |= 0b1<<7;
  delayMicroseconds(150);
  if((PINC>>3) & 0b1){
    PORTD &= ~(0b1<<7);
    delayMicroseconds(150);
    if(!((PINC>>3) & 0b1)){
      PORTD |= 0b1<<7;
      delayMicroseconds(150);
      if((PINC>>3) & 0b1) connectedCables &= ~0b100;
    } 
  }
  PORTD &= ~(0b1<<7);

  return connectedCables;
}

//updates the shift registers controlling the LEDs
void updateDisplay(uint16_t displayBits){
  PORTD &= ~(0b1<<latchPin);
  shiftOut(dataPin, clockPin, LSBFIRST, (uint8_t)(displayBits & 0xFF));
  shiftOut(dataPin, clockPin, LSBFIRST, (uint8_t)(displayBits>>8 & 0xFF));
  PORTD |= 0b1<<latchPin;
}

//finds the number of ones in a binary number
uint8_t binarySumOfOnes(uint16_t binNum){
  uint8_t sum=0;
  for(int i=0;i<16;i++){
    if((binNum>>i) & 0b1) sum++;
  }
  return sum;
}
