/*
 * controlBoxHAL.h
 *
 * Created: Nov-16
 *  Author: Kofler Joshua
 */ 


#ifndef CONTROLBOXHAL_H_
#define CONTROLBOXHAL_H_

#include <avr/io.h>

/* initialize the USART0 interface */
void USART0_init(void);
/* set MAX458 into receive mode */
void USART0_setToReceive(void);
/* set MAX458 into transmit mode */
void USART0_setToTransmit(void);
/* send a char over the USART0 interface */ 
void USART0_sendChar(unsigned char data);
/* send a string over the USART0 interface */
void USART0_sendString(unsigned char *s);
/* flush the USART0 buffer */
void USART0_flush(void);
/* receive a char over the USART0 interface */
unsigned char USART0_receive(void);
/* check if there is any error on the USART0 interface */
unsigned char USART0_isError(void);

/* initialize the USART1 interface */
void USART1_init(void);
/* set MAX458 into receive mode */
void USART1_setToReceive(void);
/* set MAX458 into transmit mode */
void USART1_setToTransmit(void);
/* send char over the USART1 interface */ 
void USART1_sendChar(unsigned char data);
/* send string over the USART1 interface */
void USART1_sendString(unsigned char *s);
/* flush the USART1 buffer */
void USART1_flush(void);
/* receive a char over the USART1 interface */
unsigned char USART1_receive(void);
/* check if there is any error on the USART1 interface */
unsigned char USART1_isError(void);

/* initialize the TWI0 interface */
void TWI0_init(void);
/* terminates the data transfer and releases the TWI0 bus */
void TWI0_stop(void);
/* issues a start condition and sends address and transfer direction */
unsigned char TWI0_start(unsigned char address);
/* send one byte to the TWI device */
unsigned char TWI0_write(unsigned char data);

/* initialize the TWI1 interface */
void TWI1_init(void);
/* terminates the data transfer and releases the TWI1 bus */
void TWI1_stop(void);
/* issues a start condition and send address and transfer direction */
unsigned char TWI1_start(unsigned char address);
/* send one byte to the TWI device */
unsigned char TWI1_write(unsigned char data);

/* initialize the EAT123 display */
void EAT123_init(void);
/* set position of the cursor */
void EAT123_setPosition(unsigned char line, unsigned char pos);
/* write char out to display on current position */
void EAT123_write(unsigned char *output, unsigned char line);

#endif /* CONTROLBOXHAL_H_ */