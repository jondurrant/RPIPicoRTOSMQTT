/*
 * MQTTInterface.h
 *
 * Abstract interface to MQTT functions. Breaks dependencies for compilation
 *
 *  Created on: 1 Nov 2021
 *      Author: jondurrant
 */

#ifndef MQTTINTERFACE_H_
#define MQTTINTERFACE_H_

#include <stdlib.h>
#include <pico/stdlib.h>

class MQTTInterface {
public:
	MQTTInterface();
	virtual ~MQTTInterface();

	/***
	 * Returns the id of the client
	 * @return
	 */
	virtual const char * getId()=0;

	/***
	 * Publish message to topic
	 * @param topic - zero terminated string. Copied by function
	 * @param payload - payload as pointer to memory block
	 * @param payloadLen - length of memory block
	 */
	virtual MQTTStatus_t pubToTopic(const char * topic, const void * payload, size_t payloadLen)=0;

	/***
	 * Close connection
	 */
	virtual void close()=0;

	/***
	 * Route a message to the router object
	 * @param topic - non zero terminated string
	 * @param topicLen - topic length
	 * @param payload - raw memory
	 * @param payloadLen - payload length
	 */
	virtual void route(const char * topic, size_t topicLen, const void * payload, size_t payloadLen)=0;

	/***
	 * Received an ack packet. Use to make sure we wait for acts before
	 * sending next request
	 * @param pkt
	 */
	virtual void mqttAck(uint16_t pkt)=0;
};

#endif /* MQTTINTERFACE_H_ */
