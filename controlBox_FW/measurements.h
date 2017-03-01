/*
 * measurements.h
 *
 * Created: 17.02.2017 20:34:45
 *  Author: paddy
 */ 


#ifndef MEASUREMENTS_H_
#define MEASUREMENTS_H_

#include "measurements_hal.h"
#include "controlBoxHAL.h"

extern timer_var_t meas_timer;

void measurement_init(void);
void measurement_process(void);

#endif /* MEASUREMENTS_H_ */