/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Joshua Eerkes
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "stm32l432xx.h"
#include "stm32l4xx.h"
#include "systick.h"
#include "waveforms.h"
#include "map.h"
#include "I2C.h"
#include "MCP4725.h"

#define square 0
#define ramp 1
#define saw 3
#define triangle 4
#define sine 5
#define randomStep 6
#define randomRamp 7

#define HSI_FREQ 16000000UL //Master Clock Frequency

static uint32_t ADCData[4];
uint32_t *waveformReading = &ADCData[0];
uint32_t *periodReading = &ADCData[1];
uint32_t *phaseReading = &ADCData[2];
uint32_t *amplitudeReading = &ADCData[3];
uint32_t waveform = 0;
uint32_t lastWaveformReading = 0;
uint32_t period = 0;
uint32_t lastPeriodReading = 0;
uint32_t phase = 0;
uint32_t amplitude = 0;
uint32_t output = 0;
uint32_t currentAmplitude = 0;
uint32_t currentPeriod = 100;
uint32_t currentPhase = 0;
uint32_t lastRandom = 0;
uint32_t newRandom = 0;
volatile uint32_t interruptPeriod = 0;
volatile uint32_t lastInterruptTime = 0; //in terms of millis()
volatile uint32_t interruptCounter = 0;
int32_t t; //master timescale
int32_t t_relative = 0; //phase-shifted timescale
int32_t last_t = 0; //previous time value
int32_t last_t_relative = 0; //previous phase-shifted time value
uint32_t lastZeroTime = 0;
int32_t hysteresisFactor = 20;
volatile bool syncMode = false;
bool syncStrike = false;
uint8_t waveformStrike = 0;

int main(void){

	//set up HSI16 as system clock
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY){};
	RCC->CFGR |= 1UL;

	RCC->CRRCR |= 0b1; //enables HSI48 (needed for RNG)

	systick_setup(HSI_FREQ); //set up systick

	SCB->CPACR |= (15UL << 20U); //set up the FPU

	//clock setup
	RCC->AHB1ENR |= 1UL; //enables DMA1 clock
	RCC->AHB2ENR |= 1UL << 18; //enables rng clock
	RCC->AHB2ENR |= 1UL << 13; //enables adc clock
	RCC->AHB2ENR |= 1UL; //enables GPIO port A clock
	RCC->AHB2ENR |= 1UL << 1; //enables GPIO port B clock
	RCC->APB1ENR1 |= 1U << 21; //enables I2C1 clock
	RCC->APB2ENR |= 1UL; //enables syscfg clock
	RCC->APB2ENR |= 1UL<<17; //enables TIM16 clock

	GPIOA->AFR[0] |= 0b1110<<6*4; //initializes port A low alternate functions (PWM16/1 on PA6)
	GPIOA->AFR[1] |= 0x44<<4; //initializes port A high alternate functions (I2C on PA9 & PA10)

	//initializes port A pin modes [][10][10][][][10][10][][][10][11][11][][11][11][00]
	GPIOA->MODER &= ~0b11; //PA0 digital input
	GPIOA->MODER &= ~(0b11<<6*2);
	GPIOA->MODER |= 0b10<<6*2; //PA6 alternate function (TIM16/1)
	GPIOA->MODER &= ~(0b111111<<8*2);
	GPIOA->MODER |= 0b1010<<9*2; //PA9-10 alternate function (I2C)
	GPIOA->MODER &= ~(0b1111<<13*2);
	GPIOA->MODER |= 0b1010<<13*2; //PA13-14 alternate function (debug)

	//initializes port B pin modes [][][][][][][][][01][01][01][01][01][][01][01]
	GPIOB->MODER &= ~0b1111;
	GPIOB->MODER |= 0b101; //PB0-1 digital output
	GPIOB->MODER &= ~(0b1111111111<<3*2);
	GPIOB->MODER |= 0b101010101<<3*2; //PB3-7 digital output

	GPIOA->OTYPER |= 0b11<<9; //initializes I2C pins (PA9-10) as open drain
	GPIOA->OSPEEDR |= 0b1010<<9*2; //initializes I2C pins as Fast Speed
	GPIOA->PUPDR |= 0b10; //initializes pulldown on PA0
	GPIOB->PUPDR = 0UL; //clears random pullup on PB4

	SYSCFG->EXTICR[0] &= ~0b111; //sets PA0 as source for EXTI0 source input
	EXTI->IMR1 |= 1UL; //unmasks interrupt request from EXTI line 0
	EXTI->RTSR1 |= 1UL; //interrupt triggered on rising edge
	NVIC->ISER[0] |= 1UL<<6; //enables EXTI0 (interrupt 6) interrupt

	I2C1->CR1 &= ~I2C_CR1_PE; //disable I2C1
	I2C1->TIMINGR |= 0x30420F13; //black magic timing stuff
	I2C1->CR1 |= I2C_CR1_PE; //enable I2C1

	RNG->CR |= 0b1<<2; //enable random number generator

	DMA1_Channel1->CPAR = (uint32_t) &(ADC1->DR); //ADC data register address
	DMA1_Channel1->CMAR = (uint32_t) &ADCData; //ADC data array address
	DMA1_Channel1->CCR |= 0b10<<10; //32-bit memory
	DMA1_Channel1->CCR |= 0b10<<8; //32-bit peripheral
	DMA1_Channel1->CNDTR |= 0x4; //4 transfers
	DMA1_Channel1->CCR |= 0b1<<7; //memory increment mode
	DMA1_Channel1->CCR |= 0b1<<5; //circular mode
	DMA1_Channel1->CCR |= 0b1; //enable DMA

	ADC1_COMMON->CCR |= 0b01<<16; //synchronous clock mode
	ADC1->CR &= ~(0b1<<29); //wake ADC from deep power down
	ADC1->CR |= 0b1<<28; //enable voltage regulator
	delay(1);
	ADC1->CR |= 0b1<<31; //start adc calibration
	while(((ADC1->CR)>>31)){} //wait until adc calibration complete
	ADC1->ISR &= ~0b1; //clear ADRDY bit
	ADC1->CR |= 0b1; //enable adc
	while((ADC1->CR & 1UL) != 1UL){}; //wait until ADC is ready
	ADC1->CFGR |= 0b1<<13; //enable continuous conversion mode
	ADC1->SQR1 |= 0x3; // 4 conversions in regular sequence
	ADC1->SQR1 |= ((0x6<<6) | (0x8<<12) | (0x9<<18) | (0xA<<24)); //channels in sequence
	ADC1->CFGR2 |= 0b0100<<5; //oversampling shift (/32)
	ADC1->CFGR2 |= 0b100<<2; //oversampling ratio (x32)
	ADC1->SMPR1 |= 0b111<<6*3 | 0b111<<8*3 | 0b111<<9*3; //Channel 6,7,9 sampling time
	ADC1->SMPR2 |= 0b111; //Channel 10 sampling time
	ADC1->CFGR |= 0b11; //enable DMA and select circular mode
	ADC1->CR |= 0b1<<2; //start converting

	TIM16->PSC |= 15UL; //counter frequency is 1MHz
	TIM16->ARR = 999UL; //PWM frequency of 1000Hz
	TIM16->CCMR1 |= 0b110<<4; //PWM mode 1
	TIM16->CCR1 = 499UL;
	TIM16->CCMR1 |= 1UL<<3; //preload enable
	TIM16->CCER |= 1UL; //enables output compare mode
	TIM16->CR1 |= 1UL<<7; //enables auto-load preload
	TIM16->BDTR |= 0b1<<15; //main output enable
	TIM16->EGR |= 1UL; //initialize all registers
	TIM16->CR1 |= 1UL; //enables timer

	//loop
	while(true){
		//keeps random noise from changing the wave
		if(*waveformReading<lastWaveformReading-293-hysteresisFactor || *waveformReading>lastWaveformReading+293+hysteresisFactor){
			waveformStrike++;
		} else waveformStrike = 0;

		if(waveformStrike>4){
			waveform = map(*waveformReading,0,4095,0,6);
			lastWaveformReading = map(waveform,0,6,0,4095);
			waveformStrike=0;
		}

		//knob is being moved
		if((int32_t)*periodReading<((int32_t)lastPeriodReading-(int32_t)hysteresisFactor) || *periodReading>(lastPeriodReading+hysteresisFactor)){
			if(syncStrike){
				syncMode = false;
				lastPeriodReading = *periodReading;
			}
			else syncStrike = true;
		} else syncStrike = false;

		amplitude = map(map(*amplitudeReading,0,4095,0,60),0,60,0,4095);

		if(!syncMode) {                                          //free oscillation mode
			period = map(*periodReading,0,4095,100,10000);
		} else {                                                 //synced oscillation mode
			period = interruptPeriod;
			if(lastInterruptTime>lastZeroTime && interruptCounter==0) lastZeroTime = lastInterruptTime;
		}

		if(currentPeriod<period) currentPeriod++;
		else if(currentPeriod>period) currentPeriod--;

		phase = map(*phaseReading,0,4095,0,currentPeriod);

		if(currentPhase<phase) currentPhase++;
		else if(currentPhase>phase) currentPhase--;

		if(currentAmplitude<amplitude) currentAmplitude++;
		else if(currentAmplitude>amplitude) currentAmplitude--;

		t = (millis()-lastZeroTime)%currentPeriod; //calculate absolute time
		if(t<last_t){
			lastZeroTime = millis()-t;
			t = (millis()-lastZeroTime)%currentPeriod;
		}

		t_relative = (t+currentPeriod-currentPhase)%currentPeriod; //calculate relative time (in terms of phase offset)

		switch(waveform){
		case 0:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<square;
			output = square_wave(t_relative, currentAmplitude, currentPeriod);
			break;
		case 1:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<ramp;
			output = ramp_wave(t_relative, currentAmplitude, currentPeriod);
			break;
		case 2:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<saw;
			output = saw_wave(t_relative, currentAmplitude, currentPeriod);
			break;
		case 3:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<triangle;
			output = triangle_wave(t_relative, currentAmplitude, currentPeriod);
			break;
		case 4:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<sine;
			output = sine_wave(t_relative, currentAmplitude, currentPeriod);
			break;
		case 5:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<randomStep;
			if(t_relative<(last_t_relative-hysteresisFactor)) {
				output = map(map(map((RNG->DR & ~0xFFFFF000),0,4095,0,currentAmplitude),0,4095,0,60),0,60,0,4095);
			}
			break;
		case 6:
			GPIOB->ODR &= ~0b11111011;
			GPIOB->ODR |= 0b1<<randomRamp;
			if(t_relative<(last_t_relative-hysteresisFactor)) {
				lastRandom = newRandom;
				newRandom = map(map(map((RNG->DR & ~0xFFFFF000),0,4095,0,currentAmplitude),0,4095,0,60),0,60,0,4095);
			}
			output = (uint32_t)((float)(((int32_t)newRandom-(int32_t)lastRandom)/(float)currentPeriod)*t_relative+(int32_t)lastRandom);
			break;
		}
		TIM16->CCR1 = map(output,0,4095,0,999); //control duty cycle of indicator LED
		MCP4725_Write((4095-output),false); //write output value
		last_t = t;
		last_t_relative = t_relative;
	}
}

void EXTI0_IRQHandler(void){
	if(millis()-lastInterruptTime>10000) interruptCounter = 1;
	else interruptCounter++;
	if(interruptCounter>=2){
		interruptPeriod = millis()-lastInterruptTime;
		interruptCounter = 0;
		syncMode = true;
	}
	lastInterruptTime = millis();
	EXTI->PR1 |= 1UL; //clears pending bit
}

