/*
 * MQTTPingTask.cpp
 *
 *  Created on: 14 Nov 2021
 *      Author: jondurrant
 */

#include "MQTTPingTask.h"
#include "MQTTDebug.h"

MQTTPingTask::MQTTPingTask() {
	// TODO Auto-generated constructor stub

}

MQTTPingTask::~MQTTPingTask() {
	// TODO Auto-generated destructor stub
}

void MQTTPingTask::setInterface(MQTTInterface * mi){
	mqttInterface = mi;
}

void MQTTPingTask::setPongTopic(char * topic){
	pongTopic = topic;
}

/***
 *  create the vtask, will get picked up by scheduler
 *
 *  */
void MQTTPingTask::start(UBaseType_t priority){
	xMessageBuffer = xMessageBufferCreate( PINGBUFFER );
	if (xMessageBuffer == NULL){
		dbg("ERROR No MQTTPingTask PINGBUFFER");
		return;
	}
	if (xMessageBuffer != NULL){
		xTaskCreate(
			MQTTPingTask::vTask,
			"MQTTping",
			512,
			( void * ) this,
			priority,
			&xHandle
		);
	}
}

/***
 * Internal function used by FreeRTOS to run the task
 * @param pvParameters
 */
void MQTTPingTask::vTask( void * pvParameters ){
	MQTTPingTask *task = (MQTTPingTask *) pvParameters;
	task->run();
}


bool MQTTPingTask::addPing(const void * payload, size_t payloadLen){
	size_t res = xMessageBufferSend(xMessageBuffer,
			payload, payloadLen, 0 );
	if (res != payloadLen){
		dbg("ERROR  MQTTPingTask::addPing failed\n");
		return false;
	}
	return true;
}


void MQTTPingTask::run(){
	char payload[MAXPINGPAYLOAD];
	for (;;){
		if (xMessageBufferIsEmpty(xMessageBuffer) == pdTRUE){
			taskYIELD();
		} else {
			size_t size = xMessageBufferReceive(xMessageBuffer,
								payload, MAXPINGPAYLOAD, 0);
			if (size > 0){
				if ((pongTopic != NULL) && (mqttInterface != NULL)){
					mqttInterface->pubToTopic(pongTopic, payload, size);
				} else {
					dbg("MQTTPingTask interface or topic not set");
				}
			}
		}
	}

}
