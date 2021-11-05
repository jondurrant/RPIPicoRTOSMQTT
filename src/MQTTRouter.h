/*
 * MQTTRouter.h
 *
 * Abstract router interface.
 * Defines iterface for MQTTTask to get messages handled by application
 *
 *  Created on: 31 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTROUTER_H_
#define MQTTROUTER_H_

#include <core_mqtt.h>
#include "MQTTInterface.h"

class MQTTRouter {
public:
	MQTTRouter();
	virtual ~MQTTRouter();

	/***
	 * Return the list of topics to subscribe to.
	 * this only gets setup once and is static
	 * @return
	 */
	 virtual MQTTSubscribeInfo_t * getSubscriptionList()=0;

	 /***
	  * Returns the number of topics in the subscription list
	  * @return
	  */
	 virtual size_t getSubscriptionCount()=0;

	 /***
	  * Route a message. Handle the message and connect to wider application
	  * @param topic - non zero terminated string
	  * @param topicLen - length of topic
	  * @param payload - memory structure of payload
	  * @param payloadLen - payload length
	  * @param interface - MQTT interface to use for any response publication
	  */
	 virtual void route(const char *topic, size_t topicLen, const void * payload, size_t payloadLen, MQTTInterface *interface)=0;



};

#endif /* MQTTROUTER_H_ */
