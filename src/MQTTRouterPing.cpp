/*
 * MQTTRouterPing.cpp
 *
 *  Created on: 31 Oct 2021
 *      Author: jondurrant
 */

#include "MQTTRouterPing.h"
#include <stdio.h>
#include <cstring>


#include "MQTTDebug.h"

const char * MQTTRouterPing::PINGTOPIC = "TNG/%s/TPC/PING";
const char * MQTTRouterPing::PONGTOPIC = "TNG/%s/TPC/PONG";

MQTTRouterPing::MQTTRouterPing() {
}

MQTTRouterPing::MQTTRouterPing(const char * id, MQTTInterface *mi) {
	init(id, mi);
}

void MQTTRouterPing::init(const char * id, MQTTInterface *mi) {
	this->id = id;

	sprintf(pingTopic, PINGTOPIC, id);
	sprintf(pongTopic, PONGTOPIC, id);

	subscriptions[ 0 ].qos = MQTTQoS1;
	subscriptions[ 0 ].pTopicFilter = pingTopic;
	subscriptions[ 0 ].topicFilterLength = strlen( pingTopic );

	pingTask.setPongTopic(pongTopic);
	pingTask.setInterface(mi);
	pingTask.start();

}

MQTTRouterPing::~MQTTRouterPing() {
	// TODO Auto-generated destructor stub
}

MQTTSubscribeInfo_t * MQTTRouterPing::getSubscriptionList(){
	return subscriptions;

}


size_t MQTTRouterPing::getSubscriptionCount(){
	return SUBSCRIPTIONS;
}

void MQTTRouterPing::route(const char *topic, size_t topicLen, const void * payload, size_t payloadLen, MQTTInterface *interface){

	dbg("MQTTRouterPing(%.*s[%d]: %.*s[%d])\n",topicLen, topic, topicLen, payloadLen, (char *)payload, payloadLen);
	if (strlen(pingTopic) == topicLen){
		if (memcmp(topic, pingTopic, topicLen)==0){

			/*
			interface->pubToTopic(pongTopic, payload, payloadLen);
			*/
			pingTask.addPing(payload, payloadLen);
		}
	}
}


