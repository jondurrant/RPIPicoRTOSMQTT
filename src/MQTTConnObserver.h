/*
 * MQTTConnObserver.h
 *
 * MQTT Connection observer
 *
 *  Created on: 30 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTCONNOBSERVER_H_
#define MQTTCONNOBSERVER_H_

#include "NetConnection.h"

class MQTTConnObserver {
public:
	MQTTConnObserver();
	virtual ~MQTTConnObserver();

	/***
	 * MQTT has been initilialised
	 */
	virtual void mqttInitialised();

	/***
	 * tcp connection has been established
	 */
	virtual void tcpConnActive();

	/***
	 * mqtt connection has been established
	 */
	virtual void mqttConnActive();

	/***
	 * Connection has closed
	 */
	virtual void connClosed();

	/***
	 * data has been sent
	 */
	virtual void dataSent();

	/***
	 * data has been received
	 */
	virtual void dataRecv();

	/***
	 * a connection error has occurred - normally with establishing connection
	 */
	virtual void connError();
};

#endif /* MQTTCONNOBSERVER_H_ */
