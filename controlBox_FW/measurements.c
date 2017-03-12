
/*
 * measurements.c
 *
 * Created: 17.02.2017 20:35:06
 *  Author: paddy
 */ 
#include "measurements.h"
#include "measurements_hal.h"
#include "controlBoxHAL.h"

static enum states{MEAS_ADV, MEAS_SIMP, WAIT} state;

extern uint16_t bounds[7];
extern uint16_t voltageDiv[4];
extern uint16_t voltages[6];
extern uint8_t statusCode;
extern uint8_t measLED_valid;
uint8_t nLED;
uint8_t meascount = 0;
timer_var_t meas_timer;

void measurement_init(void)
{
	state = WAIT;
	
	meas_timer = TIMER_SEC(1);
	
	measurement_hal_init();
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
			
			meascount = 0;
			
			get_ambient_TEMP();
			get_sensor_value(0);			
			get_all_voltages();
			get_all_div_voltages();
			
			nLED = 0;
			
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
				statusCode = 0x07;
			}
			else
			{
				statusCode = 0x00;
			}
			
			statusCode |= (nLED << 3);
			state = WAIT;
			meas_timer = TIMER_SEC(3);
						
		break;
		
		case MEAS_SIMP:		//STATE MEAS_SIMP
			
			meascount++;
			
			get_ambient_TEMP();
			get_all_div_voltages();
			
			nLED=0;
			
			for (uint8_t i = 0; i<4; i++)
			{
				//USART1_sendChar((char)(voltageDiv[i]>>8));
				//USART1_sendChar((char)(voltageDiv[i]));
				if (voltageDiv[i] < bounds[6])
				{
					for (uint8_t ii = 0; ii<6; ii++)
					{
						if (voltageDiv[i] >= bounds[ii])
						{
							nLED++;							
						}
					}
				}
			}
			
			
			if (nLED > 12)
			{
				statusCode = 0x07;
			}
			else
			{
				statusCode = 0x00;	
			}
		
			statusCode |= (nLED << 3);
			state = WAIT;
			meas_timer = TIMER_SEC(1);
			
			
		break;
		
		case WAIT:			//STATE WAIT
		
			if (meas_timer == 0)
			{
				if (meascount == 10)
				{
					state = MEAS_ADV;
				}
				else
				{
					state = MEAS_SIMP;
				}
				
			}
		break;
		
	}

}