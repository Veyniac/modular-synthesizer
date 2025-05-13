/*
 * I2C.h
 *
 *  Created on: Feb 4, 2025
 *      Author: joshu
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void I2C_send(uint8_t addr, uint8_t data[], uint8_t dataLength);

#endif /* I2C_H_ */
