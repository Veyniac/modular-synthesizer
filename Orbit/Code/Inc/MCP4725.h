/*
 * MCP4725.h
 *
 *  Created on: Feb 13, 2025
 *      Author: joshu
 */

#ifndef MCP4725_H_
#define MCP4725_H_

#include <stdint.h>
#include <stdbool.h>

void MCP4725_Write(uint32_t data, bool writeEEPROM);

#endif /* MCP4725_H_ */
