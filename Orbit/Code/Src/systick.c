/*
 * systick.c
 *
 *  Created on: Jan 31, 2025
 *      Author: joshu
 */

#include <stdint.h>
#include "stm32l432xx.h"
#include "systick.h"

	void systick_setup(uint32_t systemFrequency){
		SysTick->LOAD |= (systemFrequency / 1000U) - 1U; //sets the SysTick timer to interrupt at 1000 Hz
		SysTick->VAL = 0U;
		SysTick->CTRL |= 1UL << 2; //use AHB/1 as clock source
		SysTick->CTRL |= 1UL << 1; //enable interrupts
		SysTick->CTRL |= 1UL; //enable counter
	}

	void SysTick_Handler(void){
		SYSTEM_MS++;
	}

	uint32_t millis(void){
	    return SYSTEM_MS;
	}

	void delay(uint32_t delayMillis){
		uint32_t endMillis = millis() + delayMillis;
		while(millis()!=endMillis);
	}


