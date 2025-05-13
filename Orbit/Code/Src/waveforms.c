/*
 * waveforms.c
 *
 *  Created on: Jan 31, 2025
 *      Author: joshu
 */

#include <stdint.h>
#include <Math.h>

uint32_t sine_wave(uint32_t t, uint32_t amplitude, uint32_t period){
	return (amplitude/2.0)*(sin((6.2832/period)*t+4.7124)+1.0);
}

uint32_t square_wave(uint32_t t, uint32_t amplitude, uint32_t period){
	if(t<period/2) return amplitude;
	else return 0;
}

uint32_t saw_wave(uint32_t t, uint32_t amplitude, uint32_t period){
	return (uint32_t)(((float)amplitude/period)*(period-t));
}

uint32_t ramp_wave(uint32_t t, uint32_t amplitude, uint32_t period){
	return (uint32_t)(((float)amplitude/period)*t);
}

uint32_t triangle_wave(uint32_t t, uint32_t amplitude, uint32_t period){
	if(t<=period/2) return (uint32_t)(((float)2*amplitude/period)*t);
	else return (uint32_t)(((float)2*amplitude/period)*(period-t));
}


