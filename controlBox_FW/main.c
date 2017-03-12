/*
 * controlBoxV1.c
 *
 * Created: 20-02-17
 * Author : Kofler Joshua / Patrick Jenny
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
 #include "measurements.h"
 #include "lightcontrol.h"

 TIMERS(&meas_timer, &light_timer);
 
 /* intern stored address */
 uint16_t address = 0x0C1;
 /* intern stored toolID = 001 (handrail) */
 uint8_t toolID = 0x01;

 //////////////////////////////////////////////////////////////////////////
 // global variables

/* 4-Bit LED/sensor status code */
uint8_t statusCode; // OK not OK
/* 7 bounds */
uint16_t bounds[7];




/* 4 div voltages */
uint16_t voltageDiv[4];

/* 4 voltages */
uint16_t voltages[6];

/* 10-Bit ADC06 value (sensor) */
uint16_t sensorValue;

/* 10-Bit ADC08 value (temperature) */
int16_t temperature;

/* LED brightness */
uint8_t brightness;

/* LED mode */
uint8_t lightmode;
// OFF		          --> 0x00
// ON				  --> 0x01
// costume brightness --> 0x02
// blink			  --> 0x03

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
		 case NORMAL:			/* normal operation state */
			lightcontrol_process();
			measurement_process();
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

 //////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////// main

int main(void)
{
<<<<<<< HEAD
	bounds[0] = 1880;
	bounds[1] = 4760;
	bounds[2] = 7640;
	bounds[3] = 10520;
	bounds[4] = 13400;
	bounds[5] = 16000;
	bounds[6] = 22000;

	
	hal_init();
	measurement_init();
	lightcontrol_init();
    /* initialize main features */
	sei();
	/* initialize UART interfaces */
	
	//could be put into hal_init() @joshua from patrick ?
	USART0_init();
	USART1_init();

	USART0_setToReceive();
	USART1_setToTransmit();
	
	lightmode = 0x01;
	brightness = 50;
	
    while (1) 
    {
		
		hal_process();
		measurement_process();
		lightcontrol_process();
		
		USART1_sendChar((char)(temperature>>8));
		USART1_sendChar((char)(temperature));
		USART1_sendChar(statusCode);
		//stateMachine();
		//sendAddress();
		
=======
	/* initialize main features */
	hal_init();
	measurement_init();
	lightcontrol_init();
	sei();

    while (1) 
    {
		stateMachine();
>>>>>>> origin/master
    }
}


