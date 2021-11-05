/*
 * MQTTRouterPing.h
 *
 * Example router responding to Ping message with a pong response
 *
 *  Created on: 31 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTROUTERPING_H_
#define MQTTROUTERPING_H_

#include "MQTTRouter.h"

#define SUBSCRIPTIONS 1

class MQTTRouterPing : public MQTTRouter{
public:
	MQTTRouterPing();
	MQTTRouterPing(const char * id);
	virtual ~MQTTRouterPing();

	virtual void init(const char * id);

	virtual MQTTSubscribeInfo_t * getSubscriptionList();
	virtual size_t getSubscriptionCount();

	virtual void route(const char *topic, size_t topicLen, const void * payload, size_t payloadLen, MQTTInterface *interface);

private:
	MQTTSubscribeInfo_t subscriptions[SUBSCRIPTIONS];
	const char * id;




	static const char * PINGTOPIC;
	static const char * PONGTOPIC;
	char pingTopic[20];
	char pongTopic[20];

};

#endif /* MQTTROUTEREXP_H_ */
