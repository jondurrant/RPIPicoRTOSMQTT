/*
 * MQTTConnTask.h
 * Connection task, operates and provides interface to task managing the mqtt
 * connection
 *
 *  Created on: 30 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTCONNTASK_H_
#define MQTTCONNTASK_H_

#include "MQTTConnection.h"
#include "MQTTCommand.h"
#include <FreeRTOS.h>
#include <message_buffer.h>
#include <semphr.h>
#include <stdbool.h>

#include "MQTTConnObserver.h"
#include "MQTTRouter.h"
#include "MQTTInterface.h"

#define MQTTDEBUGSTATS


//TODO move parameters into a config file for lib version
//Publish buffer length
#define PUBBUFFER 512
//pub transfer buffer
#define PUBTRANBUFFER 200
//Queue length for mqtt commands
#define MQTTCMDQUEUELEN 10
//Reconnect time out
#define MQTTRECONDELAY 5000
//Time will wait for ack before forcing state, ms
#define MQTTACKWAITMS 5000


enum MQTTState {  Offline, OK, Wait, Ack, Recv};

class MQTTConnTask : public MQTTConnObserver, public MQTTInterface{
public:
	MQTTConnTask();
	virtual ~MQTTConnTask();

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

	/***
	 * Set credentials
	 * @param user - string pointer. Not copied so pointer must remain valid
	 * @param passwd - string pointer. Not copied so pointer must remain valid
	 * @param id - string pointer. Not copied so pointer must remain valid. I
	 * f not provide ID will be user
	 * @return lwespOK if succeeds
	 */
	lwespr_t credentials(const char * user, const char * passwd, const char * id = NULL );

	/***
	 * Connect to mqtt server
	 * @param target - hostname or ip address, Not copied so pointer must remain valid
	 * @param port - port number
	 * @param ssl - unused
	 * @return
	 */
	virtual lwespr_t connect(char * target, lwesp_port_t  port, bool ssl=false);

	/***
	 * Provide router object for any messages on a topic to provided to.
	 * This is how clients receive messages from object
	 * @param router
	 */
	virtual void setRouter(MQTTRouter *router);

	/***
	 * Get ID of the MQtT client
	 * @return
	 */
	virtual const char * getId();

	/***
	 * Publish message to topic
	 * @param topic - zero terminated string. Copied by function
	 * @param payload - payload as pointer to memory block
	 * @param payloadLen - length of memory block
	 */
	virtual void pubToTopic(const char * topic, const void * payload, size_t payloadLen);

	/***
	 * Close connection
	 * Not yet implimented
	 */
	virtual void close();

	/***
	 * Route a message to the router object
	 * @param topic - non zero terminated string
	 * @param topicLen - topic length
	 * @param payload - raw memory
	 * @param payloadLen - payload length
	 */
	virtual void route(const char * topic, size_t topicLen, const void * payload, size_t payloadLen);


	/***
	 * Observer function when the tcp connection is activated, used to trigger next step
	 */
	virtual void tcpConnActive();

	/***
	 * Observer function called when mqtt has been initalised
	 */
	virtual void mqttInitialised();

	/***
	 * Observer function called when mqtt is activated
	 */
	virtual void mqttConnActive();

	/***
	 * Observer function called when connection closes
	 */
	virtual void connClosed();

	/***
	 * Observer function called when connection error occurs
	 */
	virtual void connError();

	/***
	 * has reconnect on failure been enabled
	 * @return
	 */
	bool isReconnect() const;

	/***
	 * Set reconnect on failure
	 * @param reconnect
	 */
	void setReconnect(bool reconnect = false);

	/***
	 * Received an ack packet. Use to make sure we wait for acts before
	 * sending next request
	 * @param pkt
	 */
	void mqttAck(uint16_t pkt);


	/***
	 * Is MQTT connection online
	 * @return
	 */
	bool isOnline();

protected:
	/***
	 * Internal function to run the task from within the object
	 */
	void run();

	/***
	 * add command to queue
	 * @param cmd - command gets copied and can freed after call
	 */
	void addCmd(MQTTCommand *cmd);

	/***
	 * Local error handling, mainly in debug mode
	 * @param msg
	 */
	void errorHandling(const char * msg);

	void processQueue();


	QueueHandle_t xCmdQueue;
	TaskHandle_t xHandle = NULL;
	MQTTConnection xMQTTConn;
	bool reconnect=false;

	MQTTRouter * pRouter;
	MessageBufferHandle_t xPublishBuffer = NULL;
	char xPubTransBuf[PUBTRANBUFFER];
	SemaphoreHandle_t xSemPubTransBuf = NULL;


	static const char *  LIFECYCLE_TOPIC;

	//Needed so we will wait for acks
	uint16_t packetId = 0;
	MQTTState mqttState = OK;
	uint32_t mqttStateOk = 0;

private:
#ifdef MQTTDEBUGSTATS
	char debugStats[250] ;
	void runDebugStats();
#endif

};

#endif /* MQTTCONNTASK_H_ */
