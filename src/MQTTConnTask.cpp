/*
 * MQTTConnTask.cpp
 *
 *  Created on: 30 Oct 2021
 *      Author: jondurrant
 */

#include "MQTTConnTask.h"

#include "MQTTDebug.h"

const char *  MQTTConnTask::LIFECYCLE_TOPIC = "TNG/%s/LC";

MQTTConnTask::MQTTConnTask() {
	xMQTTConn.attach(this);
}

MQTTConnTask::~MQTTConnTask() {
	if (xHandle != NULL)
		vTaskDelete(xHandle);
	if (xCmdQueue != NULL)
		vQueueDelete(xCmdQueue);
	if (xSemPubTransBuf != NULL)
		vSemaphoreDelete(xSemPubTransBuf);
}

/***
*  create the vtask, will get picked up by scheduler
*
*  */
void MQTTConnTask::start(UBaseType_t priority){
	xCmdQueue = xQueueCreate( MQTTCMDQUEUELEN, sizeof(MQTTCommand ) );
	if (xCmdQueue == NULL){
		errorHandling("ERROR No MQTTConnTask xCmdQueue");
		return;
	}
	xPublishBuffer = xMessageBufferCreate( PUBBUFFER );
	if (xPublishBuffer == NULL){
		errorHandling("ERROR No MQTTConnTask PUBBUFFER");
		return;
	}
	xSemPubTransBuf = xSemaphoreCreateBinary();
	if (xSemPubTransBuf == NULL){
		errorHandling("ERROR No MQTTConnTask xSemPubTransBuf");
		return;
	}
	xSemaphoreGive(xSemPubTransBuf);

	if (xCmdQueue != NULL){
		MQTTCommand cmd(Init);
		addCmd(&cmd);
		xTaskCreate(
			MQTTConnTask::vTask,
			"MQTTcmd",
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
 void MQTTConnTask::vTask( void * pvParameters ){
	MQTTConnTask *task = (MQTTConnTask *) pvParameters;
	task->run();
 }

/***
* Set credentials
* @param user - string pointer. Not copied so pointer must remain valid
* @param passwd - string pointer. Not copied so pointer must remain valid
* @param id - string pointer. Not copied so pointer must remain valid. I
* f not provide ID will be user
* @return lwespOK if succeeds
*/
lwespr_t MQTTConnTask::credentials(const char * user, const char * passwd, const char * id){
	return xMQTTConn.credentials(user, passwd, id);
}

/***
* Connect to mqtt server
* @param target - hostname or ip address, Not copied so pointer must remain valid
* @param port - port number
* @param ssl - unused
* @return
*/
lwespr_t MQTTConnTask::connect(char * target, lwesp_port_t  port, bool ssl){
	xMQTTConn.setTarget(target, port, ssl);
	MQTTCommand cmd(TCPConn);
	addCmd(&cmd);
	return lwespOK;
}

/***
* Internal function to run the task from within the object
*/
void MQTTConnTask::run(){

	int i=0;

	//dbg("Initial State=%d\n", mqttState);

	for( ;; ){

#ifdef MQTTDEBUGSTATS
		if (i >= 300){
			runDebugStats();
			i=0;
		} else {
			i++;
		}

#endif


		//Make sure we don't get stuck in a weight state more more than MQTTACKWAITMS
		if (isOnline()){
			if (mqttState == OK){
				mqttStateOk = xMQTTConn.getCurrentTime();
			} else {
				if ( (xMQTTConn.getCurrentTime() - mqttStateOk) > MQTTACKWAITMS){
					mqttState = OK;
					mqttStateOk = xMQTTConn.getCurrentTime();
					dbg("EXCEEDED ACK WAIT");
				}
			}
		}

		if (mqttState == OK){ //Make sure we wait for acks
			processQueue();
		}

		if (isOnline()){
			if (mqttState == Ack || mqttState == Recv){
				mqttState = OK;
				//dbg("State=%d\n", mqttState);
			}
			xMQTTConn.MQTTprocess();
		}

		taskYIELD();
	}

}



void MQTTConnTask::processQueue(){
	MQTTStatus_t res;
	MQTTCommand cmd;
	const MQTTSubscribeInfo_t * pSubscriptionList;
	size_t subscriptionCount;

	if (uxQueueMessagesWaiting(xCmdQueue)> 0){
		if( xQueueReceive( xCmdQueue,
			 &cmd, ( TickType_t ) 10 ) == pdPASS ){
			switch(cmd.getCmd()){
				case Init:{
					//dbg("Init Cmd received\n");
					res = xMQTTConn.MQTTinit(this);
					break;
				}
				case TCPConn: {
					//dbg("TCPConn Cmd received\n");
					res = xMQTTConn.TCPconn();

					break;
				}
				case Conn: {
					//dbg("MQTT Conn Cmd received\n");
					res = xMQTTConn.MQTTconn();
					break;
				}
				case Pub: {
					//dbg("MQTT Pub command\n");
					//Make sure any pending ACKs are sent
					xMQTTConn.MQTTprocess();
					if (xSemPubTransBuf != NULL){
						if( xSemaphoreTake( xSemPubTransBuf, ( TickType_t ) 10 ) == pdTRUE ){
							size_t size = xMessageBufferReceive(  xPublishBuffer,
									xPubTransBuf, PUBTRANBUFFER, 0);
							if (size > 0){
								int l = strlen(xPubTransBuf) + 1;
								void *p = (void *)(xPubTransBuf + l);
								l = size - l;
								dbg("Publishing >%s:%.*s< %d\n",xPubTransBuf,l, (char *)p, l);
								res = xMQTTConn.pubToTopic(xPubTransBuf, p, l, packetId);
							} else {
								dbg("ERROR MQTTConnTask data missing from buffer\n");
							}
							xSemaphoreGive(xSemPubTransBuf);
							if (res == MQTTSuccess){
								mqttState = Wait;
							}
						}
					}
					//dbg("State=%d\n", mqttState);
					break;
				}
				case Sub: {
					//dbg("Subscribing to router list\n");
					if (pRouter != NULL){
						pSubscriptionList = pRouter->getSubscriptionList();
						subscriptionCount = pRouter->getSubscriptionCount();
						res = xMQTTConn.subscribe(pSubscriptionList, subscriptionCount, packetId );
						if (res == MQTTSuccess){
							mqttState = Wait;
						}
					}
					//dbg("State=%d\n", mqttState);
					break;
				}
				case Close: {
					if (xMQTTConn.close() == lwespOK ){
						res = MQTTSuccess;
					} else {
						res = MQTTServerRefused;
					}
					break;
				}
				case Reconn:{
					vTaskDelay(pdMS_TO_TICKS(MQTTRECONDELAY));
					MQTTCommand cmd(TCPConn);
					addCmd(&cmd);
					break;
				}
				default:{

				}
			}


			switch(res){
					case MQTTSuccess:{
						//dbg("succeeded\n");
						break;
					}
					case MQTTNoMemory:{
						dbg("MQTTNoMemory");
						break;
					}
					case MQTTBadParameter :{
						dbg("bad param\n");
						break;
					}
					case MQTTNoDataAvailable :{
						dbg("MQTTNoDataAvailable\n");
						break;
					}
					case MQTTServerRefused :{
						dbg("MQTTServerRefused\n");
						break;
					}
					default:{
						dbg("Other result %d\n", res);
					}
			}

		}
	}
}


/***
 * Observer function when the tcp connection is activated, used to trigger next step
 */
void MQTTConnTask::tcpConnActive(){
	dbg("TCP COnnection in place\n");
	MQTTCommand cmd(Conn);
	addCmd(&cmd);
}

/***
 * Observer function called when mqtt has been initalised
 */
void MQTTConnTask::mqttInitialised(){
	dbg("MQTT Initialised\n");
}

/***
* Observer function called when mqtt is activated
*/
void MQTTConnTask::mqttConnActive(){
	char topicBuf[20];
	char onlinePayload [] = "{\"online\":1}";
	dbg("MQTT Conn Active\n");
	mqttState = OK;

	MQTTCommand cmdSub(Sub);
	addCmd(&cmdSub);


	sprintf(topicBuf, MQTTConnTask::LIFECYCLE_TOPIC, xMQTTConn.getId() );
	pubToTopic(topicBuf, onlinePayload, strlen(onlinePayload));
}


/***
 * add command to queue
 * @param cmd
 */
void MQTTConnTask::addCmd(MQTTCommand *cmd){
	xQueueSendToBack( xCmdQueue,
	            ( void * ) cmd,
	            ( TickType_t ) 10
			);
}

/***
 * Observer function called when connection closes
 */
void MQTTConnTask::connClosed(){
	mqttState = OK;
	packetId=0;
	dbg("MQTT Connection closed\n");
	if (isReconnect()){
		MQTTCommand cmd(Reconn);
		xQueueSendToFront( xCmdQueue,
		            ( void * ) &cmd,
		            ( TickType_t ) 10
				);
		mqttState  = OK;
	}

}

/***
 * Provide router object for any messages on a topic to provided to.
 * This is how clients receive messages from object
 * @param router
 */
void MQTTConnTask::setRouter(MQTTRouter *router){
	pRouter = router;
}

/***
 * Get ID of the MQtT client
 * @return
 */
const char * MQTTConnTask::getId(){
	return xMQTTConn.getId();
}

/***
 * Publish message to topic
 * @param topic - zero terminated string. Copied by function
 * @param payload - payload as pointer to memory block
 * @param payloadLen - length of memory block
 */
void MQTTConnTask::pubToTopic(const char * topic, const void * payload, size_t payloadLen){
	size_t tl = strlen(topic) + 1;
	size_t fl = tl + payloadLen;
	if (fl < PUBTRANBUFFER){
		if (xSemPubTransBuf != NULL){
			if (xSemaphoreTake( xSemPubTransBuf, ( TickType_t ) 10 ) == pdTRUE ){
				uint8_t  *target = (uint8_t  *)xPubTransBuf;
				memcpy(target, topic, tl);
				target+= tl;
				memcpy(target, payload, payloadLen);
				size_t res = xMessageBufferSend(  xPublishBuffer,
						xPubTransBuf, fl, 0 );
				if (res != fl){
					dbg("ERROR  MQTTConnTask::pubToTopic overflow xPublishBuffer\n");
				}else {
					MQTTCommand cmdPub(Pub);
					addCmd(&cmdPub);
				}
				xSemaphoreGive(xSemPubTransBuf);
			}
		}

	} else {
		dbg("ERROR MQTTConnTask::pubToTopic too long for xPubTransBuf\n");
	}
}

/***
 * Close connection
 */
void MQTTConnTask::close(){
	setReconnect(false);
	MQTTCommand cmd(Close);
	addCmd(&cmd);
}

/***
* Route a message to the router object
* @param topic - non zero terminated string
* @param topicLen - topic length
* @param payload - raw memory
* @param payloadLen - payload length
*/
void MQTTConnTask::route(const char * topic, size_t topicLen, const void * payload, size_t payloadLen){
	mqttState = Recv;
	//dbg("MQTTConnTask::route.\n");
	if (pRouter != NULL)
		pRouter->route(topic, topicLen, payload, payloadLen, this);
	else
		dbg("ERROR MQTTConnTask::route pRouter not defined.\n");
}

/***
 * Local error handling, mainly in debug mode
 * @param msg
 */
void MQTTConnTask::errorHandling(const char * msg){
	dbg("%s\n", msg);
}

/***
* has reconnect on failure been enabled
* @return
*/
bool MQTTConnTask::isReconnect() const {
		return reconnect;
}

/***
* Set reconnect on failure
* @param reconnect
*/
void MQTTConnTask::setReconnect(bool reconnect) {
	this->reconnect = reconnect;
}

/***
 * Observer function called when connection error occurs
*/
void MQTTConnTask::connError(){
	dbg("Connection error\n");
	mqttState  = Offline;
	packetId=0;
	if (isReconnect()){
		MQTTCommand cmd(Reconn);
		xQueueSendToFront( xCmdQueue,
				            ( void * ) &cmd,
				            ( TickType_t ) 10
						);
		mqttState  = OK;
	}
}

/***
* Received an ack packet. Use to make sure we wait for acts before
* sending next request
* @param pkt
*/
void MQTTConnTask::mqttAck(uint16_t pkt){
	if (pkt == packetId){
		//dbg("Packet Ack\n");
		packetId = 0;
		mqttState = Ack;
		//dbg("State=%d\n", mqttState);
	}
}


/***
* Is MQTT connection online
* @return
*/
bool MQTTConnTask::isOnline(){
	return (mqttState != Offline);
}



#ifdef MQTTDEBUGSTATS
void MQTTConnTask::runDebugStats(){
	//vTaskGetRunTimeStats(debugStats);
	//printf("%s\n", debugStats);
	unsigned int hwm = uxTaskGetStackHighWaterMark(NULL);
	printf("High Water %d\n", hwm);
}
#endif

