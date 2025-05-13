/*
 * I2C.c
 *
 *  Created on: Feb 4, 2025
 *      Author: joshu
 */

#include <I2C.h>
#include <stdint.h>
#include "stm32l432xx.h"

void I2C_send(uint8_t addr, uint8_t data[], uint8_t dataLength){
	I2C1->CR2 = 0; //resets everything
	I2C1->CR2 |= 1UL<<25; //automatically generates a STOP condition once all bytes are transferred
	I2C1->CR2 |= dataLength<<16; //how big the data to be sent in bytes is
	I2C1->CR2 |= addr<<1; //chooses target address
	I2C1->CR2 |= 1UL<<13; //generates START condition

	for(uint8_t i=0;i<dataLength;i++){
		while((I2C1->ISR & 1UL) != 1UL){} //wait until transmit data register empty
		I2C1->TXDR = (I2C1->TXDR & 0xFFFFFF00) | data[i]; //put data in register to be transmitted
	}
	return;
}
