/*
 * lightcontrol.h
 *
 * Created: 20.02.2017 16:36:37
 *  Author: paddy
 */ 


#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_

#include "controlBoxHAL.h"

extern timer_var_t light_timer;

void linghtcontrol_init(void);

void lightcontrol_process(void);

void inv_brightness(void);

#endif /* LIGHTCONTROL_H_ */