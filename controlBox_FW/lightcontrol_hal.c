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
	DDRD  |= (1 << DDRD6); // D6 - Ausgang
	//TCCR0A = (1 << WGM01) | (1 << WGM00); //Mode select
	TCCR0A = 0xC3;
	TCCR0B = 0x02;
	//TCCR0B = (1 << WGM02) | (1 << CS01);
}

void brightness_fully_on(void)
{
	TCCR0A &= ~((1 << COM0A1) | (1<<COM0A0));
	PORTD &= ~(1 << PORTD6);
}

void brightness_fully_off(void)
{
	TCCR0A &= ~((1 << COM0A1) | (1<<COM0A0));
	PORTD |= (1<<PORTD6);
}

void set_brighntess(uint8_t val)
{
	TCCR0A |= ((1 << COM0A1) | (1<<COM0A0));
	OCR0A = val;
}

void inv_brightness(void)
{	
	PORTD^=(1<<PORTD6);
}