/*
 * lightcontrol.c
 *
 * Created: 20.02.2017 16:18:11
 *  Author: paddy
 */ 

#include "lightcontrol_hal.h"
#include "lightcontrol.h"
#include "controlBoxHAL.h"

static enum states{FULLYON, FULLYOFF, BLINK, COSTUMBRIGHTNESS} state;
	
extern uint8_t brightness;
extern  uint8_t lightmode;
extern uint8_t measLED_valid;

uint8_t act_brightness;

timer_var_t light_timer;

void linghtcontrol_init(void)
{
	state = FULLYON;
	measLED_valid = 1;
	lighcontrol_hal_init();
	brightness_fully_on();
}

void lightcontrol_process(void)
{
	switch(state)
	{
		case FULLYON:			//STATE FULLON
			if(lightmode == 0x00)
			{
				state = FULLYOFF;
				brightness_fully_off();
				measLED_valid = 0;
			}
			
			if(lightmode == 0x02)
			{
				state = COSTUMBRIGHTNESS;
				set_brighntess(brightness);
				act_brightness = brightness;
				measLED_valid = 0;
			}
			if(lightmode == 0x03)
			{
				state = BLINK;
				brightness_fully_on();
				measLED_valid = 1;
				light_timer = TIMER_SEC(1);
			}
		break;
		
		case FULLYOFF:				//STATE FULLOFF
			if(lightmode == 0x01)
			{
				state = FULLYON;
				brightness_fully_on();
				measLED_valid = 1;
			}
			if(lightmode == 0x02)
			{
				state = COSTUMBRIGHTNESS;
				set_brighntess(brightness);
				act_brightness = brightness;
				measLED_valid = 0;
			}
			if(lightmode == 0x03)
			{
				state = BLINK;
				brightness_fully_on();
				measLED_valid = 1;
				light_timer = TIMER_SEC(1);
			}
		break;
		
		case COSTUMBRIGHTNESS:				//STATE COSTUMBRIGHTNESS
			if(act_brightness != brightness)
			{
				set_brighntess(brightness);
				act_brightness = brightness;
			}
			if(lightmode == 0x00)
			{
				state = FULLYOFF;
				brightness_fully_off();
				measLED_valid = 0;
			}
			if(lightmode == 0x01)
			{
				state = FULLYON;
				brightness_fully_on();
				measLED_valid = 1;
			}
			if(lightmode == 0x03)
			{
				state = BLINK;
				brightness_fully_on();
				measLED_valid = 1;
				light_timer = TIMER_SEC(1);
			}
		break;
		
		case BLINK:				//STATE BLINK
			
			if (light_timer == 0)
			{
				inv_brightness();
				light_timer = TIMER_SEC(1);
			}
			if(lightmode == 0x00)
			{
				state = FULLYOFF;
				brightness_fully_off();
				measLED_valid = 0;
			}
			if(lightmode == 0x01)
			{
				state = FULLYON;
				brightness_fully_on();
				measLED_valid = 1;
			}
			if(lightmode == 0x02)
			{
				state = COSTUMBRIGHTNESS;
				set_brighntess(brightness);
				act_brightness = brightness;
				measLED_valid = 0;
			}
			break;
			
		break;
	}

}