/*
 * MQTTConnection.cpp
 *
 *  * Internal MQTT COnnection management. This class is used by MQTTTask to manage
 * the connection.
 * Currently defaults to TCP connection
 *
 *  Created on: 27 Oct 2021
 *      Author: jondurrant
 */

#include "MQTTConnection.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#include <MQTTDebug.h>


MQTTConnection::MQTTConnection() {
	xNetworkContext.tcpConn = &tcpConn;
	xNetworkContext.mqttTask = NULL;
	xBuffer.pBuffer = netBuffer;
	xBuffer.size = BUFFERLEN;
	tcpConn.attach(this);
}

MQTTConnection::~MQTTConnection() {
	// TODO Auto-generated destructor stub
}

/***
 * Set credentials
 * @param user - string pointer. Not copied so pointer must remain valid
 * @param passwd - string pointer. Not copied so pointer must remain valid
 * @param id - string pointer. Not copied so pointer must remain valid. I
 * f not provide ID will be user
 * @return lwespOK if succeeds
 */
lwespr_t MQTTConnection::credentials(const char * user, const char * passwd, const char * id){
	this->user = user;
	this->passwd = passwd;
	if (id != NULL){
		this->id = id;
	} else {
		this->id = user;
	}
	dbg("MQTT Credentials Id=%s, usr=%s, pwd=%s\n", this->id, this->user, this->passwd);
	return lwespOK;
}

/***
* get the client id set through credentials
* @return point to string
*/
const char * MQTTConnection::getId(){
	return id;
}

/***
* set the target endpoint for MQTT server
* @param target - host name or ip as string.  Not copied so pointer must remain valid
* @param port - port number
* @param ssl - not implemented
*/
void MQTTConnection::setTarget(const char * target, lwesp_port_t  port, bool ssl ){
	this->target = target;
	this->port = port;
	this->ssl = ssl;
}

/***
 * Connect TCP socket
 * @return
 */
MQTTStatus_t MQTTConnection::TCPconn(){

	lwespr_t res = tcpConn.connect(target, port);

	if (res == lwespOK){
		return MQTTSuccess;
	}

	return MQTTServerRefused;
}

/***
* Initialise MQTT service. Should be called before anything else
* @param interface
* @return
*/
MQTTStatus_t  MQTTConnection::MQTTinit(MQTTInterface *interface){
	MQTTStatus_t xResult;
	TransportInterface_t xTransport;

	xNetworkContext.mqttTask = interface;
	xTransport.pNetworkContext = &xNetworkContext;
	xTransport.send = MQTTConnection::tcp_send;
	xTransport.recv = MQTTConnection::tcp_recv;

	xResult = MQTT_Init( &xMQTTContext,
						 &xTransport,
						 MQTTConnection::getCurrentTime,
						 MQTTConnection::MQTTEventCallback,
						 &xBuffer );

	std::list<MQTTConnObserver *>::iterator iterator = observers.begin();
		while (iterator != observers.end()) {
		  (*iterator)->mqttInitialised();
		  ++iterator;
		}


	return xResult;

}

/***
* Connect to MQTT server
* @return
*/
MQTTStatus_t  MQTTConnection::MQTTconn(){
	MQTTStatus_t xResult;
	MQTTConnectInfo_t xConnectInfo;
	bool xSessionPresent = false;

	/* Many fields not used in this demo so start with everything at 0. */
	( void ) memset( ( void * ) &xConnectInfo, 0x00, sizeof( xConnectInfo ) );

	/* Start with a clean session i.e. direct the MQTT broker to discard any
	 * previous session data. Also, establishing a connection with clean
	 * session will ensure that the broker does not store any data when this
	 * client gets disconnected. */
	xConnectInfo.cleanSession = true;

	/* The client identifier is used to uniquely identify this MQTT client to
	 * the MQTT broker. In a production device the identifier can be something
	 * unique, such as a device serial number. */
	xConnectInfo.pClientIdentifier = id;
	xConnectInfo.clientIdentifierLength = ( uint16_t ) strlen(id);
	xConnectInfo.pUserName = user;
	xConnectInfo.userNameLength = ( uint16_t ) strlen(user);
	xConnectInfo.pPassword = passwd;
	xConnectInfo.passwordLength= ( uint16_t ) strlen(passwd);


	/* Set MQTT keep-alive period. It is the responsibility of the application
	 * to ensure that the interval between Control Packets being sent does not
	 * exceed the Keep Alive value.  In the absence of sending any other
	 * Control Packets, the Client MUST send a PINGREQ Packet. */
	xConnectInfo.keepAliveSeconds = 60;

	/* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
	 * is passed as NULL. */
	dbg("MQTT Connect \n");
	xResult = MQTT_Connect( &xMQTTContext,
							&xConnectInfo,
							NULL,
							30000U,
							&xSessionPresent );

	std::list<MQTTConnObserver *>::iterator iterator = observers.begin();
	while (iterator != observers.end()) {
	  (*iterator)->mqttConnActive();
	  ++iterator;
	}

	return xResult;
}


/***
* TCP connection observer function. Called when connectionis active
* @param c
*/
void MQTTConnection::connActive(NetConnection * c){
	std::list<MQTTConnObserver *>::iterator iterator = observers.begin();
		while (iterator != observers.end()) {
		  (*iterator)->tcpConnActive();
		  ++iterator;
		}
}


/***
 * Close connection
 * @return
 */
lwespr_t MQTTConnection::close(){
	//TODO
	return lwespOK;
}


/***
 * Static functions for requiremts of CoreMQQT TCP connection
 * Not blocking. Returns 0 if not data available
 * @param pNetworkContext - network context including link backt to TCP Connection and task
 * @param pBuffer - buffer to receive into
 * @param bytesToRecv - length of buffer
 * @return bytes read
 */
int32_t MQTTConnection::tcp_recv(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv){
	TCPConnection *pConn = (TCPConnection *) pNetworkContext->tcpConn;
	return pConn->recv((char *)pBuffer, bytesToRecv);
}

/***
 * Static functions for requiremts of CoreMQQT TCP connection. Send data
 * @param pNetworkContext - network context including link backt to TCP Connection and task
 * @param pBuffer - buffer to send from
 * @param bytesToSend - length of buffer
 * @return
 */
int32_t MQTTConnection::tcp_send(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend){
	TCPConnection *pConn = (TCPConnection *) pNetworkContext->tcpConn;
	lwespr_t res = pConn->send((const char *)pBuffer, bytesToSend);
	if (res == lwespOK){
		return bytesToSend;
	} else {
		return 0;
	}
}

/***
 * Required by CoreMQTT returns time in ms
 * @return
 */
uint32_t MQTTConnection::getCurrentTime(){
	return to_ms_since_boot(get_absolute_time ());
}


/***
* Call back function from CoreMQTT, passes control back to object
* @param pContext - MQTT context allows us to find this object
* @param pPacketInfo - packet info on ack or data packet
* @param pDeserializedInfo - data for packet
*/
void MQTTConnection::MQTTEventCallback (struct MQTTContext *pContext, struct MQTTPacketInfo *pPacketInfo, struct MQTTDeserializedInfo *pDeserializedInfo){
	//dbg("MQTTConnection::MQTTEventCallback called\n");
	if( ( pPacketInfo->type & 0xF0U ) == MQTT_PACKET_TYPE_PUBLISH )
	{
		//TopicName does not seem to be zero terminated, so not really a string
		const char * topic = pDeserializedInfo->pPublishInfo->pTopicName;
		const char * payload = (const char *)pDeserializedInfo->pPublishInfo->pPayload;
		size_t payloadLen = pDeserializedInfo->pPublishInfo->payloadLength;
		size_t topicLen = pDeserializedInfo->pPublishInfo->topicNameLength;

		MQTTInterface *interface = (MQTTInterface *)pContext->transportInterface.pNetworkContext->mqttTask;
		if (interface != NULL){
			interface->route(topic, topicLen, payload, payloadLen);
		} else {
			dbg("ERROR MQTTConnection::MQTTEventCallback Interface not defined\n");
		}
		//dbg("Received topic=%s, payload=%s\n",topic, payload );
	}
	else
	{
		//prvMQTTProcessResponse( pxPacketInfo, pxDeserializedInfo->packetIdentifier );
		//dbg("Response packet %d\n", pDeserializedInfo->packetIdentifier);

		MQTTInterface *interface = (MQTTInterface *)pContext->transportInterface.pNetworkContext->mqttTask;
		interface->mqttAck(pDeserializedInfo->packetIdentifier);
	}
}



/***
 * TCP connection observer function. Called when connection closes
 * @param c
 */
void MQTTConnection::connClosed(NetConnection * c){

	std::list<MQTTConnObserver *>::iterator iterator = observers.begin();
	while (iterator != observers.end()) {
	  (*iterator)->connClosed();
	  ++iterator;
	}
}

/***
* TCP connection observer function. Called when connection cannot be opened
* due to error
* @param c
*/
void MQTTConnection::connError(NetConnection * c){
	std::list<MQTTConnObserver *>::iterator iterator = observers.begin();
	while (iterator != observers.end()) {
	  (*iterator)->connError();
	  ++iterator;
	}
}


/***
 * Publish payload to a topic
 * @param topic - must to zero terminated string
 * @param payload - can be any data in mem
 * @param payloadLen - length of data
 * @param pktId - packet id sent
 * @return MQTTSuccess if ok
 */
MQTTStatus_t MQTTConnection::pubToTopic(const char * topic, const void * payload,
		size_t payloadLen, uint16_t &pktId){
	MQTTStatus_t xResult;
	MQTTPublishInfo_t xMQTTPublishInfo;
	uint16_t packetId;

	if (packetId == 0){

		packetId = MQTT_GetPacketId( &xMQTTContext );

		/* Some fields are not used by this demo so start with everything at 0. */
		( void ) memset( ( void * ) &xMQTTPublishInfo, 0x00, sizeof(
															xMQTTPublishInfo ) );
		xMQTTPublishInfo.qos = MQTTQoS1;
		xMQTTPublishInfo.retain = false;
		xMQTTPublishInfo.pTopicName = topic;
		xMQTTPublishInfo.topicNameLength = ( uint16_t ) strlen( topic );
		xMQTTPublishInfo.pPayload = payload;
		xMQTTPublishInfo.payloadLength = payloadLen;

		/*DEBUG
		dbg("MQTTCon:Pub(%d) >%s:%.*s< (%d, %d)\n",packetId, topic, payloadLen, (char *)payload,
					xMQTTPublishInfo.topicNameLength, payloadLen);
		*/

		/* Send PUBLISH packet. Packet ID is not used for a QoS0 publish. */
		xResult = MQTT_Publish( &xMQTTContext, &xMQTTPublishInfo, packetId );
		if (xResult == MQTTSuccess){
			pktId = packetId;
		}
	} else {
		dbg("ERROR MQTTConn:pub called before ack\n");
		xResult = MQTTIllegalState;
	}
	return xResult;
}


/***
 * Attach an observer which will be notified of events
 * @param observer
 */
void MQTTConnection::attach(MQTTConnObserver *observer){
	observers.push_back(observer);
}

/***
 * Detach observer
 * @param observer
 */
void MQTTConnection::detach(MQTTConnObserver *observer){
	observers.remove(observer);
}


/***
 * Execute MQTTprocess to handle receiving and keep alives
 */
void MQTTConnection::MQTTprocess(){
	MQTT_ProcessLoop( &xMQTTContext, MQTTPROCESSMS );
}


MQTTStatus_t MQTTConnection::subscribe(const MQTTSubscribeInfo_t * pSubscriptionList,
            size_t subscriptionCount, uint16_t &pktId){
	uint16_t packetId;
	MQTTStatus_t xResult;

	if (packetId == 0){
		packetId = MQTT_GetPacketId( &xMQTTContext );

		/*DEBUG
		dbg("MQTTCon:Sub(%d) %d>%.*s< %d\n",packetId, subscriptionCount,
				pSubscriptionList[0].topicFilterLength ,
				pSubscriptionList[0].pTopicFilter,
				pSubscriptionList[0].topicFilterLength );
		*/

		xResult = MQTT_Subscribe( &xMQTTContext, pSubscriptionList,
				subscriptionCount, packetId );
		if (xResult == MQTTSuccess){
			pktId = packetId;
		}
	} else {
		dbg("ERROR MQTTConn:sub called before ack\n");
		xResult = MQTTIllegalState;
	}
	return xResult;

}



