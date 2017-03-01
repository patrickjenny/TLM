/*
 * measurements_hal.h
 *
 * Created: 17.02.2017 20:30:22
 *  Author: paddy
 */ 

#include <stdint.h>

#ifndef MEASUREMENTS_HAL_H_
#define MEASUREMENTS_HAL_H_

uint16_t get_chip_TEMP(void);

uint16_t get_all_voltages(void);

void get_all_div_voltages(void);

uint16_t get_sensor_value(uint8_t sensortype);

#endif /* MEASUREMENTS_HAL_H_ */