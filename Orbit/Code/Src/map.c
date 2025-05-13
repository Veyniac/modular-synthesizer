/*
 * map.c
 *
 *  Created on: Feb 10, 2025
 *      Author: joshu
 */

#include "map.h"
#include <math.h>

int32_t map(int32_t num, int32_t fromLow, int32_t fromHigh, int32_t toLow, int32_t toHigh){
	int32_t result = round(((double)(toHigh-toLow)/(fromHigh-fromLow))*(num-fromLow)+toLow);
	return result;
}
