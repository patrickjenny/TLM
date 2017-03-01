/*
 * protocolAbstraction.c
 *
 * Created: 20-Feb-17 2:24:41 PM
 *  Author: Kofler Joshua
 */
 //////////////////////////////////////////////////////////////////////////
 // important intern saved variables / includes
 
 #include <avr/io.h>

 #include "protocolAbstraction.h"
 #include "controlBoxHAL.h"

 extern uint8_t toolID;	/* declaration in main.c */

 //////////////////////////////////////////////////////////////////////////
 // bus system buffer
 #define BUFFERSIZE		100
 extern uint8_t bufferU0[BUFFERSIZE];
 extern uint8_t writePointerU0;
 extern uint8_t readPointerU0;

 extern uint8_t ubfferU1[BUFFERSIZE];
 extern uint8_t writePointerU1;
 extern uint8_t readPointerU1;

 //////////////////////////////////////////////////////////////////////////
 // different modes

 /* START/STOP Condition */
 uint8_t START = 0xA0;
 uint8_t STOP = 0x09;

#define MODEADR				1
 extern uint16_t address;
 uint8_t addressFrame[3];

#define MODESTD				2
 extern uint8_t statusCode;	/* OK / not OK */

#define MODESPE				3
 extern uint16_t bounds[7];
 extern uint16_t voltageDiv[4];
 extern uint16_t sensorValue;
 extern uint16_t temperature;
 
#define MODETST				4
 uint8_t helloCode = 0x02;
 uint8_t helloPacket[3];

#define MODEHEL				5

#define MODECTR				6
 extern uint8_t brightness;
 extern uint8_t lightmode;

 void bus_createAddressForNext(void)
 {
	addressFrame[0] = (START + MODEADR);
	addressFrame[1] = (toolID << 5) + (uint8_t)((address + 1) >> 4);
	addressFrame[2] = ((uint8_t)((address + 1) << 4) + STOP);
 }

 void bus_createHello(void)
 {
	helloPacket[0] = (START + MODEHEL);
	helloPacket[1] = (address >> 1);
	helloPacket[2] = ((address << 7) + (helloCode << 5) + STOP);
 }

 void calculateReadPointer(void)
 {
	if (readPointerU0 >= (BUFFERSIZE - 1))
		readPointerU0 = 0;
	else readPointerU0++;
 }

void bus_process()
{
	uint8_t mode;
	/* 
	 * execute when the UART0 buffer is filled with three new bytes or a special flag 
	 * is set
	 * the query also considers the step over
	 */
	if (writePointerU0 >= (readPointerU0 + 3) || writePointerU0 < readPointerU0)
	{
		if((bufferU0[readPointerU0]&0xF0) == START)
		{	
			mode = (bufferU0[readPointerU0]&0x0F);
			calculateReadPointer();

			if((bufferU0[readPointerU0]&0xE0) == (toolID << 5))
			{
				switch(mode)
				{
					/* addressing */
					case MODEADR:
						address = ((bufferU0[readPointerU0]&0x1F) << 4);
						calculateReadPointer();

						address += (bufferU0[readPointerU0] >> 4);
						calculateReadPointer();

						bus_createAddressForNext();
						bus_createHello();

						USART1_sendString(addressFrame);
					break;

					/* standard query */
					case MODESTD:
					break;

					/* special query */
					case MODESPE:
						if (address == ((bufferU0[readPointerU0]&0x1F) << 3) + 
												((bufferU0[readPointerU0 + 1]&0xE0) >> 4))
						{
						
						}
					break;

					/* connection test */
					case MODETST:
					break;

					/* control device */
					case MODECTR:
					break;

				}
			}
		}
		else readPointerU0++;
	}
}