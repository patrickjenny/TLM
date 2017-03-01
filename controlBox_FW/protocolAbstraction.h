/*
 * protocolAbstraction.h
 *
 * Created: 20-Feb-17 10:50:52 AM
 *  Author: Kofler Joshua
 */ 


#ifndef PROTOCOLABSTRACTION_H_
#define PROTOCOLABSTRACTION_H_

/* initialize and apply main parameters */
void bus_init(void);
/* evaluate the incoming frames */
void bus_process(void);

/* generate HELLO-Packet in dependency of the device address */
void bus_generateHelloPacket(void);

#endif /* PROTOCOLABSTRACTION_H_ */