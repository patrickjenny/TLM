
/*
 * measurements.c
 *
 * Created: 17.02.2017 20:35:06
 *  Author: paddy
 */ 
#include "measurements.h"
#include "Global_Timer.h"
#include "measurements_hal.h"

#include <avr/io.h>

static enum states{MEAS_ADV, MEAS_SIMP, WAIT} state;

extern uint16_t bounds[7];
extern uint16_t voltageDiv[4];
extern uint8_t statusCode;
extern uint8_t measLED_valid;
uint8_t nLED;
timer_var_t meas_timer;

void measurement_init(void)
{
	state = WAIT;
	
	DDRD |= (1<<PORTD7);
	PORTD &= ~(1<<PORTD7); 
}

void measurement_process(void)
{
	if (!measLED_valid)
	{
		meas_timer = TIMER_SEC(1);
		state = WAIT;
	}
	
	switch(state)
	{
		case MEAS_ADV:		//State MEAS_ADV
			
			get_sensor_value(0);			meas_timer = TIMER_MINUTE(1);
			get_all_voltages();
			get_all_div_voltages();
			for (uint8_t i = 0; i<4; i++)
			{
				if (voltageDiv[i] < bounds[6])
				{
					for (uint8_t ii = 0; ii<6; ii++)
					{
						if (voltageDiv[i] > bounds[ii])
						{
							nLED++;
						}
					}
				}
			}
			
			if (nLED > 12)
			{
				statusCode = 0x03;
			}
			else
			{
				statusCode = 0x00;
			}
			
			statusCode |= (nLED << 3);
			state = WAIT;
		
		break;
		
		case MEAS_SIMP:		//STATE MEAS_SIMP

			PORTD |= (1<<PORTD7);
			
						
			get_all_div_voltages();
			for (uint8_t i = 0; i<4; i++)
			{
				if (voltageDiv[i] < bounds[6])
				{
					for (uint8_t ii = 0; ii<6; ii++)
					{
						if (voltageDiv[i] > bounds[ii])
						{
							nLED++;
						}
					}
				}
			}
		
			if (nLED > 12)
			{
				statusCode = 0x03;
			}
			else
			{
				statusCode = 0x00;	
			}
		
			statusCode |= (nLED << 3);
			state = WAIT;
			meas_timer = TIMER_SEC(1);
			PORTD &= ~(1<<PORTD7); 
		break;
		
		case WAIT:			//STATE WAIT
		
			if (meas_timer == 0)
			{
				state = MEAS_SIMP;
			}
		
		break;
		
	}

}