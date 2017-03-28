/*
 * controlBox_Measurements.c
 *
 * Created: 17.02.2017 12:23:42
 * Author : paddy
 */ 
#include "controlBoxHAL.h"

#include <avr/io.h>
#include <stdint.h>

extern  uint16_t voltageDiv[4];
extern uint16_t voltages[6];
extern uint16_t sensorValue;
extern uint16_t temperature;
 
/*********************************************************************
ADC-Messung des übergebenen Kanals (chan)
10Bit, Polling, 1.1V Referenz, Mittelung von 4 Werten
*********************************************************************/

void measurement_hal_init(void)
{
	TWI1_init();
	DS1621_init();
}

uint16_t read_adc_10(char channel)
{
	//  0x00 - ADC0
	//  0x01 - ADC1
	//  0x02 - ADC2
	//  0x03 - ADC3
	//  0x04 - ADC4
	//  0x05 - ADC5
	//  0x06 - ADC6
	//  0x07 - ADC7
	//  0x08 - Temp Sensor
	uint16_t val=0;
	ADMUX=0xC0 | (channel & 0x0F);			//URef=1.1V, untersten 4 Bits = channel
	ADCSRA=0x86;							//10Bit, einmalige Messung, kein Interrupt, Systemclock/64
	for (int i=0;i<4;i++)
	{
		ADCSRA|=1<<ADSC;				//Start der Messung
		while(!(ADCSRA & (1<<ADIF)));		//Warten aufs Ende der Messung
		val+=ADC;
	}
	val = val/4;	        
	return val;
}

uint16_t get_chip_TEMP(void)
{
	uint16_t temp = 0;
	// not evalluated
	temp = read_adc_10(0x08);
	return temp;
}

void get_ambient_TEMP(void)
{
	temperature = read_DS1621();
}

void get_all_voltages(void)
{
	voltages[0] = read_adc_10(0x00) *25.741;
	voltages[1] = read_adc_10(0x01) *25.741;
	voltages[2] = read_adc_10(0x02) *25.741;
	voltages[3] = read_adc_10(0x03) *25.741;
	voltages[4] = read_adc_10(0x04) *25.741;
	voltages[5] = read_adc_10(0x05) *25.741;
}

void get_all_div_voltages(void)
{
	voltageDiv[0] = (read_adc_10(0x04) - read_adc_10(0x00)) *25.741;
	voltageDiv[1] = (read_adc_10(0x05) - read_adc_10(0x01)) *25.741;
	voltageDiv[2] = (read_adc_10(0x04) - read_adc_10(0x02)) *25.741;
	voltageDiv[3] = (read_adc_10(0x05) - read_adc_10(0x03)) *25.741;
}

void get_sensor_value(uint8_t sensortype)
{
	uint16_t val = 0;
	val = read_adc_10(0x06) * (1100/1024);
	if (sensortype)
	{
		sensorValue = val * (1000/47); // Messung µA ueber einen 47R Widerstand
	}
	else
	{
		sensorValue = sensorValue*11; //Messung mv
	}

}

