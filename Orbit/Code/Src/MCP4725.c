/*
 * MCP4725.c
 *
 *  Created on: Feb 13, 2025
 *      Author: joshu
 */

#include <stdbool.h>
#include <stdint.h>
#include "I2C.h"

#define MCP4725_ADDR 0x60 //DAC address

static uint8_t dataArray[6];

void MCP4725_Write(uint32_t data, bool writeEEPROM) {
	dataArray[0] = 0b1<<6;
	if(writeEEPROM) dataArray[0] |= 0b1<<5;
	dataArray[1] = (uint8_t) (data>>4 & 0xFF);
	dataArray[2] = (uint8_t) (data<<4 & 0xFF);
	dataArray[3] = dataArray[0];
	dataArray[4] = dataArray[1];
	dataArray[5] = dataArray[2];

	I2C_send(MCP4725_ADDR, dataArray, 6U);
}
