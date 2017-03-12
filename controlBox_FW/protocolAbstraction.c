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
 #define BUFFERSIZE		512
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

  uint8_t receiveCount = 3;
  uint8_t receiveFlag	= 0;

#define MODEADR				1
 extern uint16_t address;

#define MODESTD				2

#define MODEDEV				3

#define MODESPE				4
 
#define MODETST				5

#define MODEHEL				6

#define MODECTR				7
uint16_t varValue;
uint8_t varID;
	#define sensorValueID		1
	extern uint16_t sensorValue;

//////////////////////////////////////////////////////////////////////////
// bus system buffer

uint8_t buffer[BUFFERSIZE];
uint8_t writePointer;
uint8_t readPointer;

void bus_process(void)
{
	/* check flags (jump to) */

	/* 
	 * execute when the buffer is filled with three new bytes or a special 
	 * flag is set
	 * the query also considers the step over
	 *
	 * current problems
	 * -------------------------------------------------------------------
	 * timing of addressing does not work fine
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
					if ((buffer[readPointer]&0xE0) == toolID)
					{
						/* when the received toolID matches intern toolID */
						temp = ((buffer[readPointer]&0x1F) << 4);
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
					if (readPointerU0 >= (BUFFERSIZE - 1))
						readPointerU0 = 0;
					else readPointerU0++;
				break;


				case MODESPE: /* special query */
					/* first 3-bit include toolID */
					if ((buffer[readPointer]&0xE0) == toolID)
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
						else;

						frame[36] = STOP;
					}
					else
					{

					}
					USART1_sendStringWL(frame, 37);
				break;


				case MODECTR: /* control device */
				break;

				default:
				/* Your code here */
				break;
			}
		}
		else readPointer++;
	 }
}

void bus_process_1(void)
{
	/* when flag is set jump to  */
	if (receiveFlag == 1)
		goto recstr;
	if (receiveFlag == 2)
		goto finframe1;
	if (receiveFlag == 3)
		goto finframe2;

	//////////////////////////////////////////////////////////////////////////
	// current problems
	// *** found ->		the first if query makes some problems when the buffer is filled with new bytes and a step over occures
	// *** found ->		the while isnt correct (step over is also not considered)

	/* 
	 * execute when the UART0 buffer is filled with three new bytes or a special flag 
	 * is set
	 * the query also considers the step over
	 */

	 /* ( writePointer - readPointer + BUFFERSIZE ) % BUFFERSIZE */

	 if (((writePointerU0 - readPointerU0 + BUFFERSIZE) % BUFFERSIZE) >= 3)
	 {
		/* first 4-bit should be the START condition */
		if ((bufferU0[readPointerU0]&0xF0) == START)
		{
			/* other lower 4-bit include mode */
			mode = (bufferU0[readPointerU0]&0x0F);
			/* increase readPointer (consider step over) */
			if (readPointerU0 >= (BUFFERSIZE - 1))
				readPointerU0 = 0;
			else readPointerU0++;
			/* switch procedure for each mode */
			switch (mode)
			{
				case MODEADR: /* addressing */
					/* next three bits represent the toolID  */
					if (((bufferU0[readPointerU0]&0xE0) >> 5) == toolID)
					{	
						/* address is stored in two bytes */
						temp = ((bufferU0[readPointerU0]&0x1F) << 4);
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						/* create/store received address */
						address = temp + (bufferU0[readPointerU0] >> 4);

						/* create frame for next device */
						frame[0] = START + MODEADR;
						frame[1] = (toolID << 5) + (uint8_t)((address + 1) >> 4);
						frame[2] = (uint8_t)((address + 1) << 4) + STOP;			
					}
					else
					{
						/* send frame unchanged to the next device */
						frame[0] = START + MODEADR;
						frame[1] = bufferU0[readPointerU0];
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						frame[2] = bufferU0[readPointerU0];
					}
					/* send frame to the next device */
					USART1_sendStringWL(frame, 3);

					/* reset the readPointer */
					if (readPointerU0 >= (BUFFERSIZE - 1))
						readPointerU0 = 0;
					else readPointerU0++;
				break;






				case MODESPE: /* special query */
					/* next three bits represent the toolID */
					if (((bufferU0[readPointerU0]&0xE0) >> 5) == toolID)
					{
						/* address is stored in two bytes */
						temp = ((bufferU0[readPointerU0]&0x1F) << 4);
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						temp += (bufferU0[readPointerU0] >> 4);

						/* create frame for next device */
						frame[0] = START + MODESPE;
						frame[1] = (toolID << 5) + (uint8_t)(temp >> 4);
						frame[2] = (uint8_t)(temp << 4);
						/* check the received address */
						if (temp == address)	
						{
							/* add device information */
							frame[3] = 0xAF;
							frame[4] = 0xFE;
							// ...
							for (uint8_t xy = 5; xy < 34; xy++)
							{
								frame[xy] = 0x01;
							}
							// ...
							frame[34] = 0xAF;
							frame[35] = 0xFE;
						} 
						else goto recstr;

						frame[36] = STOP;
					}
					else
					{
						/* send frame unchanged to the next device */
						frame[0] = START + MODESPE;
						frame[1] = bufferU0[readPointerU0];
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						frame[2] = bufferU0[readPointerU0];

						/* initialize receive/store */
						/* jump point */
						recstr:
						/* set flag */
						receiveFlag = 1;

						// !!! check while loop 
						/* when a new byte is received */
						while((((writePointerU0 - readPointerU0 + BUFFERSIZE) % BUFFERSIZE) > 0) && receiveCount < 36)
						{
							/* increase readPointer (consider step over) */
							if (readPointerU0 >= (BUFFERSIZE - 1))
								readPointerU0 = 0;
							else readPointerU0++;
							/* store received bytes */
							frame[receiveCount] = bufferU0[readPointerU0];
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
					USART1_sendStringWL(frame, 36);
				break;








				case MODECTR: /* set a specific variable to a specific value (uint16_t) */
					if ((bufferU0[readPointerU0] >> 5) == toolID)
					{
						/* translate address */
						temp = ((bufferU0[readPointerU0]&0xE0) << 4);
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						temp += ((bufferU0[readPointerU0]&0xF0) >> 4);

						frame[0] = START + MODECTR;
						frame[1] = (toolID << 5) + (uint8_t)(temp >> 4);
						frame[2] = (uint8_t)(temp << 4);

						/* check address (address = 0 or address = intern address) */
						if (temp == address || temp == 0)
						{
							varID = ((bufferU0[readPointerU0]&0x0F) << 4);

							finframe1:
							/* set flag */
							receiveFlag = 2;

							while((((writePointerU0 - readPointerU0 + BUFFERSIZE) % BUFFERSIZE) > 0) && receiveCount < 6)
							{
								/* increase readPointer (consider step over) */
								if (readPointerU0 >= (BUFFERSIZE - 1))
									readPointerU0 = 0;
								else readPointerU0++;
								/* store received bytes */
								frame[receiveCount] = bufferU0[readPointerU0];
								/* increase count */
								receiveCount++;
							}
							if (receiveCount < 6)
								return;
							/* reset flag */
							receiveFlag = 0;
							/* get varID & varValue from frame */
							varID = ((frame[3]&0xF0) >> 4);
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
						}
						else goto finframe2;
					}
					else
					{
						frame[0] = START + MODECTR;
						frame[1] = bufferU0[readPointerU0];
						/* increase readPointer (consider step over) */
						if (readPointerU0 >= (BUFFERSIZE - 1))
							readPointerU0 = 0;
						else readPointerU0++;
						frame[2] = bufferU0[readPointerU0];

						/* wait till next three bytes are received */
						finframe2:
						/* set flag */
						receiveFlag = 3;

						while((((writePointerU0 - readPointerU0 + BUFFERSIZE) % BUFFERSIZE) > 0) && receiveCount < 6)
						{
							/* increase readPointer (consider step over) */
							if (readPointerU0 >= (BUFFERSIZE - 1))
								readPointerU0 = 0;
							else readPointerU0++;
							/* store received bytes */
							frame[receiveCount] = bufferU0[readPointerU0];
							/* increase count */
							receiveCount++;
						}
						if (receiveCount < 6)
							return;
						/* reset flag */
						receiveFlag = 0;
					}
					USART1_sendStringWL(frame, 6);
				break;


				default:
				break;
			}
		}
		else readPointerU0++;
	 }
}

