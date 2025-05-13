/*
 * systick.h
 *
 *  Created on: Jan 31, 2025
 *      Author: joshu
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

volatile static uint32_t SYSTEM_MS;

	void systick_setup(uint32_t systemFrequency);

	void SysTick_Handler(void);

	uint32_t millis(void);

	void delay(uint32_t delayMillis);

#endif /* SYSTICK_H_ */
