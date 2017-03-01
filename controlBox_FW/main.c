/*
 * controlBoxV1.c
 *
 * Created: 20-Feb-17 10:09:00 AM
 * Author : Kofler Joshua
 */ 
//////////////////////////////////////////////////////////////////////////
// important intern saved variables / includes
 #define F_CPU 16000000UL

 #include <avr/io.h>
 #include <stdint.h>
 #include <avr/interrupt.h>

 #include <util/delay.h>

 #include "protocolAbstraction.h"
 #include "controlBoxHAL.h"

 uint16_t address = 0x0C1;
 
 uint8_t toolID = 0x01;	/* intern toolID = 001 */

 //////////////////////////////////////////////////////////////////////////
 // global variables

 /* 4-Bit LED/sensor status code */
 uint8_t statusCode;
 /* 7 bounds */
 uint16_t bounds[7];
 /* 4 div voltages */
 uint16_t voltageDiv[4];
 /* 10-Bit ADC06 value (sensor) */
 uint16_t sensorValue;
 /* 10-Bit ADC08 value (temperature) */
 uint16_t temperature;
 /* LED brightness */
 uint8_t brightness;
 /* LED mode */
 uint8_t lightmode;
 /* bool (if measurement is possible) */
 uint8_t measLED_valid;

 //////////////////////////////////////////////////////////////////////////
 // state machine

 /* important definitions for the state machine */
 typedef enum { NORMAL, BUSSYSTEM } state_t;
 state_t state = NORMAL;

 void stateMachine()
 {
	 switch (state)
	 {
		 case NORMAL:	/* normal operation state */
		 /* Your code here */
		 break;

		 case BUSSYSTEM:		/* bus evaluation state */
			bus_process();
			state = NORMAL;
		 break;

		 default:
			state = NORMAL;		/* reset the stateMachine */
		 break;
	 }
 }

 /////////////////////////
 #/////////////////////////////////////////////////
 // bus system buffer

 #define BUFFERSIZE		100		/* receive buffer size */
 uint8_t bufferU0[BUFFERSIZE];	/* receive buffer UART0 */
 uint8_t writePointerU0 = 0;
 uint8_t readPointerU0 = 0;

 uint8_t bufferU1[BUFFERSIZE]; /* receive buffer UART1 */
 uint8_t writePointerU1 = 0;
 uint8_t readPointerU1 = 0;

ISR(USART0_RX_vect)
{
	bufferU0[writePointerU0] = UDR0;
	state = BUSSYSTEM;	/* change state / start evaluation */

	if (writePointerU0 >= (BUFFERSIZE - 1))
		writePointerU0 = 0;
	else writePointerU0++;
}

ISR(USART1_RX_vect)
{
	bufferU1[writePointerU1] = UDR1;
	state = BUSSYSTEM;	/* change state / start evaluation */

	if(writePointerU1 >= (BUFFERSIZE - 1))
		writePointerU1 = 0;
	else writePointerU1++;
}

//////////////////////////////////////////////////////////////////////////

void sendAddress(void)
{
	USART1_sendChar(0xA1);
	_delay_ms(2);
	USART1_sendChar(0x2A);
	_delay_ms(2);
	USART1_sendChar(0xA9);
	_delay_ms(2);
}

int main(void)
{
    /* initialize main features */
	sei();
	/* initialize UART interfaces */
	USART0_init();
	USART1_init();

	USART0_setToReceive();
	USART1_setToTransmit();

    while (1) 
    {
		stateMachine();
		//sendAddress();
    }
}

