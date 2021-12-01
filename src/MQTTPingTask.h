/*
 * MQTTPingTask.h
 *
 *  Created on: 14 Nov 2021
 *      Author: jondurrant
 */

#ifndef MQTTPINGTASK_H_
#define MQTTPINGTASK_H_

#include <stdlib.h>
#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>
#include <stdbool.h>
#include "MQTTInterface.h"

#define PINGBUFFER 250
#define MAXPINGPAYLOAD 20

class MQTTPingTask {
public:
	MQTTPingTask();
	virtual ~MQTTPingTask();

	void setInterface(MQTTInterface * mi);

	void setPongTopic(char * topic);

	/***
	 *  create the vtask, will get picked up by scheduler
	 *
	 *  */
	void start(UBaseType_t priority = tskIDLE_PRIORITY);

	/***
	 * Internal function used by FreeRTOS to run the task
	 * @param pvParameters
	 */
	static void vTask( void * pvParameters );


	bool addPing(const void * payload, size_t payloadLen);

protected:
	/***
	 * Internal function to run the task from within the object
	 */
	void run();

private:
	MessageBufferHandle_t xMessageBuffer = NULL;
	MQTTInterface *mqttInterface = NULL;
	TaskHandle_t xHandle = NULL;
	char * pongTopic = NULL;

};

#endif /* MQTTPINGTASK_H_ */
