#include <MD_MAX72xx.h>
#include <EEPROM.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  1
#define DATA_PIN  11 
#define CS_PIN  10 
#define CLK_PIN  13  

const int trackPin = 2;
const int stepPin = 3;
const int addPin = 4;
const int memoryPin = 5;
const int modePin = 6;
const int dataPin = 7;  //SER
const int clockPin = 8; //SRCLK
const int latchPin = 9; //RCLK
volatile int step = 0;
volatile int track = 0;
uint8_t beatMatrix[32];
uint8_t displayMatrix[8];
uint8_t LED_Counter = 0;
bool pointerOn = false;
bool lastState;


MD_MAX72XX display = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void setup() {
  for(int i=0;i<32;i++){
    beatMatrix[i] = B00000000;
  }

  pinMode(trackPin,INPUT_PULLUP);
  pinMode(stepPin,INPUT);
  pinMode(modePin,INPUT);
  pinMode(memoryPin,INPUT_PULLUP);
  pinMode(addPin,INPUT_PULLUP);
  pinMode(dataPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(latchPin,OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(stepPin), incrementStep, RISING);
  attachInterrupt(digitalPinToInterrupt(trackPin), incrementTrack, FALLING);

  registerWrite(0);
  display.begin();
  display.control(MD_MAX72XX::controlRequest_t::INTENSITY,MAX_INTENSITY/4);
  display.clear();
  updateDisplay(false);  
}

void loop() {
  switch(digitalRead(modePin)){
    case HIGH:                            //play mode
      if(lastState == LOW){
        step = 0;
        track = 0;
        lastState = HIGH;
      }
      if(!digitalRead(memoryPin)){
        readFromMemory();
        step = 0;
      }
      if(beatMatrix[step] & 00000001 == 1){
        step = 0;
      }
      if(digitalRead(stepPin)){
        registerWrite(beatMatrix[step]);  
      } else {
        registerWrite(0);                 
      }
      updateDisplay(false);
      break;
  
    case LOW:                             //program mode
      if(lastState == HIGH){
        step = 0;
        track = 0;
        lastState = LOW;
        registerWrite(0);
      }
      if(!digitalRead(memoryPin)){
        writeToMemory();
      }
      if(!digitalRead(addPin)){
        if((beatMatrix[step] & (B10000000 >> track)) > 0){
          beatMatrix[step] &= (~(B10000000 >> track));          //if the bit here is on, turn it off
        } else {
          beatMatrix[step] |= (B10000000 >> track);             //if it is off, turn it on
        }
        delay(250);                                            //prevents the button from double triggering
        if(!digitalRead(addPin)){                              //if the button is held down for 1.5 seconds, clear the beat matrix
          delay(250);
          if(!digitalRead(addPin)){
            delay(500);
            if(!digitalRead(addPin)){
              delay(1000);
              if(!digitalRead(addPin)){
                clearMatrix();
                updateDisplay(true);
                delay(500);
                break;
              }
            }
          }
        }
      }
      updateDisplay(true);
      break;
  }
}

void registerWrite(uint8_t registerValue){
  digitalWrite(latchPin,LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, registerValue);
  digitalWrite(latchPin, HIGH);
}

void incrementStep(){
  if(step>=31){
    step = 0;
  } else step+=1;
}

void incrementTrack(){
  if(track>=7){
    track = 0;
  } else track+=1;
}

void clearMatrix(){
  for(int i=0;i<32;i++){
      beatMatrix[i] = 0;
    }
    step = 0;
    track = 0; 
}

void readFromMemory(){
  for(int i=0;i<32;i++){
    beatMatrix[i] = EEPROM[i];
  }
  step = 0;
  track = 0;
}

void writeToMemory(){
  for(int i=0;i<32;i++){
    EEPROM.update(i, beatMatrix[i]);
  }  
}

void updateDisplay(bool progMode){
  if(progMode){
    LED_Counter++;
    if(LED_Counter>50) {
      LED_Counter = 0;
      pointerOn = !pointerOn;
    }
    int block = (step/8)*8;
    int blockStep = step-block;
    for(int i=0;i<8;i++){
      displayMatrix[i] = beatMatrix[block+i];
    }
    if(pointerOn){
      displayMatrix[blockStep] |= (B10000000 >> track);
    }
    for(int r=0;r<8;r++){
      display.setColumn(r,displayMatrix[r]);
    }  
  } else {
    for(int i=0;i<8;i++){
      int j = i+step;
      if(j>31){
        j -= 31;
      }
      displayMatrix[i] = beatMatrix[j];
    }
    for(int r=0;r<8;r++){
      display.setColumn(r,displayMatrix[r]);
    }
  }
}

