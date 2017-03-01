/*
 * controlBoxHAL.c
 *
 * Created: Nov-16
 *  Author: Kofler Joshua
 */ 

 /* Define CPU frequency in Hz here if not defined in 'main.c' */
 #ifndef F_CPU
 #define F_CPU 16000000L
 #endif

#include "controlBoxHAL.h"
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include <util/twi.h>
#include <util/delay.h>

////////////////////////////////////////////////////////////////////////// USART0 interface

static uint8_t timer_800us=0;

ISR(TIMER2_COMPA_vect)
{
	timer_800us++;
}

void hal_init(void)
{
	/* init global Timer */
	TCCR2A = 0x02; //CTC Mode, Prescaler 128, no output mode
	TCCR2B = 0x05;
	OCR2A = 99; // setting period to 800us
	TIMSK2 = 0x02; //Output Compare A Match Interrupt Enable
}

void hal_process(void)
{
	uint8_t index = 0;
	int32_t timer_interval;

	TIMSK2&=~0x02; // disable 800us timer
	if (timer_800us)
	{
		timer_interval=timer_800us*800;
		while (hal_timers[index] != 0)
		{
			if ((*(timer_var_t*)hal_timers[index] != 0) && (*(timer_var_t*)hal_timers[index] != TIMER_DISABLED))
			{
				if (*(timer_var_t*)(hal_timers[index]) <= timer_interval)
				{
					*(timer_var_t*)hal_timers[index] = 0;
				}
				else
				{
					*(timer_var_t*)hal_timers[index] -= timer_interval;
				}
			}
			index++;
		}
		timer_800us=0;

	}
	TIMSK2|=0x02; // enable 800us timer
}

void USART0_init(void) {
	/* Set baud rate to 9600 */
	UBRR0H = 0x00;
	UBRR0L = 0x67;
	/* Enable receiver, transmitter and interrupt*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	/* Set frame format: 8data, 1stop bit*/
	UCSR0C = (1<<UCSZ01)|(3<<UCSZ00);
}

void USART0_setToReceive(void) {
	/* Set the direction of PIND4 to output */
	DDRD |= 0x10;
	/* Set PIND4 to LOW */
	PORTD &= 0xEF; // 0b11101111 // PIND4 = 0

	/* Disable transmitter and enable interrupt*/
	UCSR0B &= (~(1<<TXEN0));
	UCSR0B |= (1<<RXCIE0);
	/* Set the direction of PIND1 to input */
	DDRD &= 0xFD;
}

void USART0_setToTransmit(void) {
	/* Set the direction of PIND4 to output */
	DDRD |= 0x10;
	/* Set PIND4 to HIGH */
	PORTD |= 0x10; // 0b00010000

	/* Enable transmitter and disable interrupt */
	UCSR0B |= (1<<TXEN0);
	UCSR0B &= ~(1<<RXCIE0);
}

void USART0_sendChar(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART0_sendString(unsigned char *s) {
	while(*s)
	{
		USART0_sendChar(*s);
		_delay_ms(2);
		s++;

	}
}

void USART0_flush(void) {
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0))
		dummy = UDR0;
}

unsigned char USART0_receive(void) {
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}

////////////////////////////////////////////////////////////////////////// USART1 interface

/* initialize the UART1 interface */
void USART1_init(void) {
	/* Set baud rate */
	UBRR1H = 0x00;
	UBRR1L = 0x67;
	/* Enable receiver, transmitter and interrupt*/
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1<<UCSZ01)|(3<<UCSZ00);
}

void USART1_setToReceive(void) {
	/* Set the direction of PINB4 to output */
	DDRB |= 0x20;
	/* Set PIND4 to LOW */
	PORTB &= 0xDF; // 0b11101111 // PIND4 = 0

	/* Disable transmitter and enable interrupt */
	UCSR1B &= (~(1<<TXEN1));
	UCSR1B |= (1<<RXCIE1);
	/* Set the dircetion of PINB3 to input */
	DDRB &= 0xF7;
}

void USART1_setToTransmit(void) {
	/* Set the direction of PIND4 to output */
	DDRB |= 0x20;
	/* Set PIND4 to HIGH */
	PORTB |= 0x20; // 0b00010000

	/* Enable transmitter and disable interrupt */
	UCSR1B |= (1<<TXEN1);
	UCSR1B &= ~(1<<RXCIE1);
}

void USART1_sendChar(unsigned char data) {
	/* Wait for empty transmit buffer */
	while ( !(UCSR1A & (1<<UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void USART1_sendString(unsigned char *s) {
	while(*s)
	{
		USART1_sendChar(*s);
		_delay_ms(2);
		s++;
	}
}

void USART1_flush(void) {
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC0))
		dummy = UDR1;
}

unsigned char USART1_receive(void) {
	/* Wait for data to be received */
	while( !(UCSR1A & (1<<RXC1)));
	/* Get and return received data from buffer  */
	return UDR1;
}

////////////////////////////////////////////////////////////////////////// TWI0 interface
/* I2C clock in Hz */
#define SCL0_CLOCK	100000L

void TWI0_init(void)
{
	/* No prescaler */
	TWSR0 = 0;
	/* Should be >10 for stable operation */
	TWBR0 = ((F_CPU / SCL0_CLOCK) - 16) / 2;
}

void TWI0_stop(void)
{
	/* Send STOP condition */
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	/* Wait until STOP condition is executed and TWI0 bus released */
	while(TWCR0 & (1<<TWSTO));
}

unsigned char TWI0_start(unsigned char address)
{
	/* Send START condition */
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	/* Wait until the transmission is completed */
	while (!(TWCR0 & (1<<TWINT)));
	/* Check value of TWI0 Status Register. Mask prescaler bits. */
	if ((TWSR0 & 0xF8) != TW_START)
		return 1;
	/* Send device address */
	TWDR0 = address;
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	/* Wait until the transmission is completed */
	while (!(TWCR0 & (1<<TWINT)));
	/* Check TWI0 Status Register. Mask prescaler bits. */
	if (((TWCR0 & 0xF8) != TW_MT_SLA_ACK) && (TWCR0 & 0xF8) != TW_MR_SLA_ACK)
		return 1;
	else
		return 0;
}

unsigned char TWI0_write(unsigned char data)
{	
	/* Send data */
	TWDR0 = data;
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	/* Wait until the transmission is completed */
	while (!(TWCR0 & (1<<TWINT)));
	/* Check TWI0 Status Register. Mask prescaler bits */
	if ((TWSR0 & 0xF8) != TW_MT_DATA_ACK)
		return 1;
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////// TWI1 interface
/* I2C clock in Hz */
#define SCL1_CLOCK	100000L

void TWI1_init(void)
{
	/* No prescaler */
	TWSR1 = 0;
	/* Should be >10 for stable operation */
	TWBR1 = ((F_CPU / SCL1_CLOCK) - 16) / 2;
}

void TWI1_stop(void)
{
	/* Send STOP condition */
	TWCR1 = (1<<TWINT1) | (1<<TWEN1) | (1<<TWSTO1);
	/* Wait until STOP condition is executed and TWI1 bus released */
	while(TWCR1 & (1<<TWSTO1));
}

unsigned char TWI1_start(unsigned char address)
{
	/* Send START condition */
	TWCR1 = (1<<TWINT1) | (1<<TWSTA1) | (1<<TWEN1);
	/* Wait until the transmission is completed */
	while (!(TWCR1 & (1<<TWINT1)));
	/* Check value of TWI1 Status Register. Mask prescaler bits. */
	if ((TWCR1 & 0xF8) != TW_START)
		return 1;
	/* Send device address */
	TWDR1 = address;
	TWCR1 = (1<<TWINT1) | (1<<TWEN1);
	/* Wait until the transmission is completed */
	while (!(TWCR1 & (1<<TWINT1)));
	/* Check TWI1 Status Register. Mask prescaler bits. */
	if (((TWCR1 & 0xF8) != TW_MT_SLA_ACK) && (TWCR1 & 0xF8) != TW_MR_SLA_ACK)
		return 1;
	else
		return 0;
}

unsigned char TWI1_write(unsigned char data)
{
	/* Send data */
	TWDR1 = data;
	TWCR1 = (1<<TWINT1) | (1<<TWEN1);
	/* Wait until the transmission is completed */
	while (!(TWCR1 & (1<<TWINT1)));
	/* Check TWI0 Status Register. Mask prescaler bits */
	if ((TWCR1 & 0xF8) != TW_MT_DATA_ACK)
		return 1;
	else
		return 0;
}

////////////////////////////////////////////////////////////////////////// Display
#define devAddress_EAT123	0b01110100

void EAT123_init(void)
{
	/* Send START condition to the Slave */
	TWI0_start(devAddress_EAT123);
	_delay_us(50);
	/* Send a control byte for function set */
	TWI0_write(0x00);
	_delay_us(50);
	/* Function set */
	TWI0_write(0x2E);
	_delay_us(50);
	/* Clear display */
	TWI0_write(0x01);
	_delay_ms(2);	// the display needs 165 cycles to clean the display
	/* go home */
	TWI0_write(0x02);
	_delay_us(50);
	/* Display on/off control */
	TWI0_write(0x0C);
	_delay_us(50);
	/* Entry mode set */
	TWI0_write(0x06);
	_delay_us(50);
	/* Send STOP condition */
	TWI0_stop();
}

void EAT123_write(unsigned char *output, unsigned char line)
{
	/* Send START condition to the Slave */
	TWI0_start(devAddress_EAT123);
	/* Send a control byte for function set */
	TWI0_write(0x00);
	/* Choose line */
	switch (line)
	{
		case 1:		/* Line one */
			TWI0_write(0xC0);
			break;
		case 2:		/* Line two */
			TWI0_write(0xA0);
			break;
		case 3:		/* Line three */
			TWI0_write(0x80);
			break;
		default:	/* If theres a problem write in line one */
			TWI0_write(0xC0);
			break;
	}
	/* Send a STOP condition*/
	TWI0_stop();
	_delay_us(100);

	/* Send a START condition */
	TWI0_start(devAddress_EAT123);
	_delay_us(100);
	/* For writing to DDRAM, RS must be set to 1. */
	/* Therefor a control byte ist needed */
	TWI0_write(0x40);

	/* write out the entire string */
	for(unsigned char count = 0; count < 12; count++)
	{
		/* write out last char first */
		TWI0_write(output[11 - count]);
		_delay_us(100);
	}
	/* Send a STOP condition*/
	TWI0_stop();
}
