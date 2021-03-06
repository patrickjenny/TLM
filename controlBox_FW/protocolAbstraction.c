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
 #define BUFFERSIZE		150
 extern uint8_t bufferU0[BUFFERSIZE];
 extern uint8_t writePointerU0;
 extern uint8_t readPointerU0;

 extern uint8_t ubfferU1[BUFFERSIZE];
 extern uint8_t writePointerU1;
 extern uint8_t readPointerU1;

 //////////////////////////////////////////////////////////////////////////
 // different modes
 uint8_t mode;

 /* temp variable */
 uint16_t temp;

 /* START/STOP Condition */
 uint8_t START = 0xA0;
 uint8_t STOP = 0x09;

 uint8_t frame[37];

  uint16_t receiveCount = 3;
  uint16_t receiveLength = 0;
  uint8_t receiveFlag	= 0;

#define MODEADR				1
 extern uint16_t address;

#define MODESTD				2

#define MODEDEV				3
 uint8_t deviceSTART = 0x50;
 uint8_t deviceSTOP = 0x06;

#define MODESPE				4
 
#define MODETST				5

#define MODEHEL				6

#define MODECTR				7
uint16_t varValue;
uint8_t varID;
	#define sensorValueID		1
	uint16_t sensorValue = 0xDADA;
	#define brightnessID		2
	uint8_t brightness = 0xAA;

//////////////////////////////////////////////////////////////////////////
// bus system buffer

extern uint8_t buffer[BUFFERSIZE];
extern uint8_t writePointer;
extern uint8_t readPointer;

void bus_process(void)
{
	/* check flags (jump to) */
	if (receiveFlag == 1)
		goto receivesend;

	if (receiveFlag == 4)
		goto receivestore1;
	if (receiveFlag == 2)
		goto receivestore2;
	if (receiveFlag == 3)
		goto receivestore3;

	/* 
	 * execute when the buffer is filled with three new bytes or a special 
	 * flag is set
	 * the query also considers the step over
	 *
	 * current problems
	 * -------------------------------------------------------------------
	 * standard query -> when toolID does not match		i dont know how long i will wait.. the address could 
	 *
	 */
	 if (((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 2)
	 {
		/* first 4-bit should contain a START condition */
		if ((buffer[readPointer]&0xF0) == START)
		{
			/* lower 4-bits represent mode */
			mode = (buffer[readPointer]&0x0F);
			/* increase readPointer (consider step over) */
			if (readPointer >= (BUFFERSIZE - 1))
				readPointer = 0;
			else readPointer++;
			/* switch action for each mode */
			switch (mode)
			{
				case MODEADR: /* addressing */
					/* first 3-bit include toolID */
					if (((buffer[readPointer]&0xE0) >> 5) == toolID)
					{
						/* when the received toolID matches intern toolID */
						temp = ((uint16_t)(buffer[readPointer]&0x1F) << 4);
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						/* store received address */
						address = temp + (buffer[readPointer] >> 4);

						/* create frame for next device */
						frame[0] = (START + MODEADR);
						frame[1] = (toolID << 5) + (uint8_t)((address + 1) >> 4);
						frame[2] = ((uint8_t)((address + 1) << 4) + STOP);
					}
					else
					{
						/* recreate frame for next device */
						frame[0] = (START + MODEADR);
						frame[1] = buffer[readPointer];
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						frame[2] = buffer[readPointer];
					}
					/* send frame */
					USART1_sendStringWL(frame, 3);

					/* reset readPointer (consider step over) */
					if (readPointer >= (BUFFERSIZE - 1))
						readPointer = 0;
					else readPointer++;
				break;

				case MODESTD: /* standard query */
					/* initialize standard query */
					receiveCount = 3; receiveLength = 0; temp = 9;
					
					/* first 3-bit include toolID */
					if (((buffer[readPointer]&0xE0) >> 5) == toolID)
					{
						varID = (buffer[readPointer] << 3);
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						varID += (buffer[readPointer] >> 5);
						/* set length */
						receiveLength = ((buffer[readPointer]&0x07) << 8);
						
						/* reset readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;

						/* create frame for next device */
						frame[0] = (START + MODESTD);
						frame[1] = ((toolID << 5) + (varID >> 3));
						frame[2] = (varID << 5);

						/* add device packet */
						frame[4] = (deviceSTART + MODEDEV);
						frame[5] = ((toolID << 5) + (uint8_t)(address >> 4));
						frame[6] = (uint8_t)(address << 4);
						switch (varID)
						{
							case sensorValueID:	/* -> 1 */
								varValue = sensorValue;
							break;
							case brightnessID:	/* -> 2 */
								varValue = brightness;
							break;

							default:
								/* fault -> 0x00 */
								varValue = 0x00;
							break;
						}
						frame[6] += (uint8_t)(varValue >> 12);
						frame[7] = (uint8_t)(varValue >> 4);
						frame[8] = ((varValue << 4) + deviceSTOP);
						/* receive following device packets */
						goto receivesend;
					}
					else
					{
						frame[0] = START + MODESTD;
						frame[1] = buffer[readPointer];
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						frame[2] = (buffer[readPointer]&0xE0);
						/* set length */
						receiveLength = ((buffer[readPointer]&0x07) << 8);
						/* set flag */
						temp = 4;

						/* reset readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;

						receivesend:
						/* set flag */
						receiveFlag = 1;

						if ((((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 0) && receiveCount == 3);
						{
							receiveLength += buffer[readPointer];
							/* switch procedure (toolID match / do not) */
							if (temp > 4)
							{
								frame[2] += ((receiveLength + 1) >> 8);
								frame[3] = (receiveLength + 1);
							}
							else
							{
								frame[2] += (receiveLength >> 8);
								frame[3] = receiveLength;
							}
							/* send frame */
							USART1_sendStringWL(frame, temp);
							/* increase count */
							receiveCount++;
							/* increase readPointer (consider step over) */
							if (readPointer >= (BUFFERSIZE - 1))
								readPointer = 0;
							else readPointer++;
						}

						while((((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 0) && receiveCount < (receiveLength * 5 + 4))
						{
							/* store received bytes */
							USART1_sendChar(buffer[readPointer]);
							/* increase count */
							receiveCount++;
							/* increase readPointer (consider step over) */
							if (readPointer >= (BUFFERSIZE - 1))
								readPointer = 0;
							else readPointer++;
						}
						/* wait till the receive is complete */
						if (receiveCount < (receiveLength * 5 + 4))
							return;

						/* reset flag */
						receiveFlag = 0;
						/* finish frame */
						USART1_sendChar(STOP);
					}
				break;


				case MODESPE: /* special query */
					/* set receive count */
					receiveCount = 3;

					/* first 3-bit include toolID */
					if (((buffer[readPointer]&0xE0) >> 5) == toolID)
					{
						/* when the received toolID matches intern toolID */
						temp = ((buffer[readPointer]&0x1F) << 4);
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						/* store received address */
						temp += (buffer[readPointer] >> 4);

						/* create frame for next device */
						frame[0] = START + MODEADR;
						frame[1] = (toolID << 5) + (uint8_t)(temp >> 4);
						frame[2] = (uint8_t)(temp << 4);
						/* check received address */
						if (temp == address)
						{
							/* add device information */
							frame[3] = 0xAF;
							frame[4] = 0xFE;
							// ...
							for (uint8_t n = 5; n < 34; n++)
							{
								frame[n] = 0x01;
							}
							// ...
							frame[34] = 0xAF;
							frame[35] = 0xFE;
						}
						else goto receivestore1;
						/* finish frame */
						frame[36] = STOP;
					}
					else
					{
						frame[0] = START + MODESPE;
						frame[1] = buffer[readPointer];
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						frame[2] = buffer[readPointer];

						receivestore1:
						/* set flag */
						receiveFlag = 4;
						/* when a new byte is received */
						while((((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 1) && receiveCount < 36)
						{
							/* increase readPointer (consider step over) */
							if (readPointer >= (BUFFERSIZE - 1))
								readPointer = 0;
							else readPointer++;
							/* store received bytes */
							frame[receiveCount] = buffer[readPointer];
							/* increase count */
							receiveCount++;
						}
						/* wait till the receive is complete */
						if (receiveCount < 36)
							return;
						/* reset flag */
						receiveFlag = 0;

						/* finish frame */
						frame[36] = STOP;
					}
					USART1_sendStringWL(frame, 37);

					/* reset readPointer (consider step over) */
					if (readPointer >= (BUFFERSIZE - 1))
						readPointer = 0;
					else readPointer++;
				break;


				case MODECTR: /* control device */
					/* set receive count */
					receiveCount = 3;

					/* first 3-bit include toolID */
					if (((buffer[readPointer]&0xE0) >> 5) == toolID)
					{
						/* when the received toolID matches intern toolID */
						temp = ((buffer[readPointer]&0x1F) << 4);
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						/* store received address */
						temp += (buffer[readPointer] >> 4);

						frame[0] = START + MODECTR;
						frame[1] = (toolID << 5) + (uint8_t)(temp >> 4);
						frame[2] = buffer[readPointer];

						/* check received address */
						if (temp == 0 || temp == address)
						{
							/* wait till the next two bytes are received */
							receivestore2:
							/* set flag */
							receiveFlag = 2;

							/* when a new byte is received */
							while((((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 1) && receiveCount < 6)
							{
								/* increase readPointer (consider step over) */
								if (readPointer >= (BUFFERSIZE - 1))
									readPointer = 0;
								else readPointer++;
								/* store received bytes */
								frame[receiveCount] = buffer[readPointer];
								/* increase count */
								receiveCount++;
							}
							if (receiveCount < 6)
								return;
							/* reset flag */
							receiveFlag = 0;

							/* get varID & varValue from frame */
							varID = ((frame[2]&0x0F) << 4) + ((frame[3]&0xF0) >> 4);

							varValue = ((frame[3]&0x0F) << 12);
							varValue += (frame[4] << 4);
							varValue += (frame[5] >> 4);

							/* set variable in dependency of varID */
							switch (varID)
							{
								case sensorValueID:
								sensorValue = varValue;
								break;
								default:
								/* Your code here */
								break;
							}
						} else goto receivestore3;
					}
					else
					{
						frame[0] = START + MODECTR;
						frame[1] = buffer[readPointer];
						/* increase readPointer (consider step over) */
						if (readPointer >= (BUFFERSIZE - 1))
							readPointer = 0;
						else readPointer++;
						frame[2] = buffer[readPointer];

						receivestore3:
						/* set flag */
						receiveFlag = 2;

						/* when a new byte is received */
						while((((writePointer - readPointer + BUFFERSIZE) % BUFFERSIZE) > 1) && receiveCount < 6)
						{
							/* increase readPointer (consider step over) */
							if (readPointer >= (BUFFERSIZE - 1))
								readPointer = 0;
							else readPointer++;
							/* store received bytes */
							frame[receiveCount] = buffer[readPointer];
							/* increase count */
							receiveCount++;
						}
						if (receiveCount < 6)
							return;
						/* reset flag */
						receiveFlag = 0;
					}
					USART1_sendStringWL(frame, 6);

					/* reset readPointer (consider step over) */
					if (readPointer >= (BUFFERSIZE - 1))
						readPointer = 0;
					else readPointer++;
				break;

				default:
				/* Your code here */
				break;
			}
		}
		else readPointer++;
	 }
}