
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
			
			get_ambient_TEMP();
			get_sensor_value(0);			
			get_all_voltages();
			
			//////////////////////////////////////////////////////////////////////////
			//TEST//
			
			//USART1_sendChar((char)(voltages[0]>>8));
			//USART1_sendChar((char)(voltages[0]));
			//USART1_sendChar((char)(voltages[1]>>8));
			//USART1_sendChar((char)(voltages[1]));
			//USART1_sendChar((char)(voltages[2]>>8));
			//USART1_sendChar((char)(voltages[2]));
			//USART1_sendChar((char)(voltages[3]>>8));
			//USART1_sendChar((char)(voltages[3]));
			//USART1_sendChar((char)(voltages[4]>>8));
			//USART1_sendChar((char)(voltages[4]));
			//USART1_sendChar((char)(voltages[5]>>8));
			//USART1_sendChar((char)(voltages[5]));
			
			//uint16_t adc03;
			//adc03 = read_adc_10(0x03)*25.741;
			//USART1_sendChar((char)(adc03>>8));
			//USART1_sendChar((char)(adc03));
			//
			//uint32_t adc13;
			//adc13 = (uint32_t)read_adc_10(0x03)*(1100*647)/(1024*27);
			//USART1_sendChar((char)(adc13>>8));
			//USART1_sendChar((char)(adc13));
			//
			//uint16_t adc23;
			//adc23 = read_adc_10(0x03)/1024*1100/27*647;
			//USART1_sendChar((char)(adc23>>8));
			//USART1_sendChar((char)(adc23));
			
			//TEST ENDE//
			//////////////////////////////////////////////////////////////////////////
			
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
				state = MEAS_SIMP;
			}
		

		break;
		
	}

}