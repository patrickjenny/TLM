/*
 * lightcontrol_hal.c
 *
 * Created: 19.02.2017 00:26:30
 *  Author: paddy
 */

#include <avr/io.h>
#include <stdint.h>


void lighcontrol_hal_init(void) //Initialisierung Timer
{
	//Set Pin PD6 as output
	DDRD  |= (1 << DDRD6);
	//PWM Mode
	TCCR0A = 0xC3; // 0xC3 = 0b11000011
	TCCR0B = 0x02; // 0x02 = 0b00000010
}

void brightness_fully_on(void)
{
	//DISABLE Timer Output and set PIN PD6 low
	TCCR0A &= ~((1 << COM0A1) | (1<<COM0A0));
	PORTD &= ~(1 << PORTD6);
}

void brightness_fully_off(void)
{
	//DISABLE Timer Output and set PIN PD6 high
	TCCR0A &= ~((1 << COM0A1) | (1<<COM0A0));
	PORTD |= (1<<PORTD6);
}

void set_brighntess(uint8_t val)
{
	// ENABLE Timer Output and set OCR Register
	TCCR0A |= ((1 << COM0A1) | (1<<COM0A0));
	OCR0A = val;
}

void inv_brightness(void)
{	
	//ínvert Pin PD6
	PORTD^=(1<<PORTD6);
}