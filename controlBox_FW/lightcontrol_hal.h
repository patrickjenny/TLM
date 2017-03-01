/*
 * lightcontrol_hal.h
 *
 * Created: 19.02.2017 00:41:24
 *  Author: paddy
 */ 


#ifndef LIGHTCONTROL_HAL_H_
#define LIGHTCONTROL_HAL_H_

#include "Global_Timer.h"
#include <stdint.h>

void lighcontrol_hal_init();

void brightness_fully_on();

void brightness_fully_off();

void set_brighntess(uint8_t val);



#endif /* LIGHTCONTROL_HAL_H_ */