/*
 * waveforms.h
 *
 *  Created on: Jan 31, 2025
 *      Author: joshu
 */

#ifndef WAVEFORMS_H_
#define WAVEFORMS_H_

#include <stdint.h>

uint32_t sine_wave(uint32_t t, uint32_t amplitude, uint32_t period);

uint32_t square_wave(uint32_t t, uint32_t amplitude, uint32_t period);

uint32_t saw_wave(uint32_t t, uint32_t amplitude, uint32_t period);

uint32_t ramp_wave(uint32_t t, uint32_t amplitude, uint32_t period);

uint32_t triangle_wave(uint32_t t, uint32_t amplitude, uint32_t period);

#endif /* WAVEFORMS_H_ */
