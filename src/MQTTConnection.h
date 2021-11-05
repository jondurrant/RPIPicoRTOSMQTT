/*
 * MQTTConnection.h
 *
 * Internal MQTT COnnection management. This class is used by MQTTTask to manage
 * the connection.
 * Currently defaults to TCP connection
 *
 *  Created on: 27 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTCONNECTION_H_
#define MQTTCONNECTION_H_

#include "NetConnection.h"
#include "TCPConnection.h"
#include "TCPConnectionObserver.h"
#include "MQTTConnObserver.h"
#include "MQTTInterface.h"
#include <stdlib.h>
#include <core_mqtt.h>
#include <stdbool.h>

//Network buffer length.
//TODO Move this out to a config file library version
#define BUFFERLEN 1024

//processing time to block within MQTTprocess
//TODO move this to a config file for lib version
#define MQTTPROCESSMS 10



class MQTTConnection : public TCPConnectionObserver{
public:
	/***
	 * Constructor
	 */
	MQTTConnection();

	/***
	 * Destructor
	 */
	virtual ~MQTTConnection();

	/***
	 * Set credentials
	 * @param user - string pointer. Not copied so pointer must remain valid
	 * @param passwd - string pointer. Not copied so pointer must remain valid
	 * @param id - string pointer. Not copied so pointer must remain valid. I
	 * f not provide ID will be user
	 * @return lwespOK if succeeds
	 */
	virtual lwespr_t credentials(const char * user, const char * passwd, const char * id = NULL );

	/***
	 * get the client id set through credentials
	 * @return point to string
	 */
	virtual const char * getId();


	/***
	 * set the target endpoint for MQTT server
	 * @param target - host name or ip as string.  Not copied so pointer must remain valid
	 * @param port - port number
	 * @param ssl - not implemented
	 */
	virtual void setTarget(const char * target, lwesp_port_t  port, bool ssl=false );


	/***
	 * Close connection
	 * @return
	 */
	virtual	lwespr_t close();

	/***
	 * Attach an observer which will be notified of events
	 * @param observer
	 */
	void attach(MQTTConnObserver *observer);

	/***
	 * Detach observer
	 * @param observer
	 */
	void detach(MQTTConnObserver *observer);

	/***
	 * Publish payload to a topic
	 * @param topic - must to zero terminated string
	 * @param payload - can be any data in mem
	 * @param payloadLen - length of data
	 * @param pktId - packet id sent
	 * @return MQTTSuccess if ok
	 */
	virtual MQTTStatus_t pubToTopic(const char * topic, const void * payload, size_t payloadLen, uint16_t &pktId);

	/***
	 *Subscribe to topic list
	 * @param pSubscriptionList - array of topics
	 * @param subscriptionCount - count of topics
	 * @param pktId - packet id sent
	 * @return MQTTSucess if ok
	 */
	virtual MQTTStatus_t subscribe(const MQTTSubscribeInfo_t * pSubscriptionList,
	            size_t subscriptionCount, uint16_t &pktId);





	/***
	 * Call back function from CoreMQTT, passes control back to object
	 * @param pContext - MQTT context allows us to find this object
	 * @param pPacketInfo - packet info on ack or data packet
	 * @param pDeserializedInfo - data for packet
	 */
	static void MQTTEventCallback (struct MQTTContext *pContext, struct MQTTPacketInfo *pPacketInfo, struct MQTTDeserializedInfo *pDeserializedInfo);

	/***
	 * TCP connection observer function. Called when connectionis active
	 * @param c
	 */
	virtual void connActive(NetConnection * c);

	/***
	 * TCP connection observer function. Called when connection closes
	 * @param c
	 */
	virtual void connClosed(NetConnection * c);

	/***
	 * TCP connection observer function. Called when connection cannot be opened
	 * due to error
	 * @param c
	 */
	virtual void connError(NetConnection * c);




	/***
	 * Initialise MQTT service. Should be called before anything else
	 * @param interface
	 * @return
	 */
	MQTTStatus_t  MQTTinit(MQTTInterface *interface);

	/***
	 * Connect to MQTT server
	 * @return
	 */
	MQTTStatus_t  MQTTconn();

	/***
	 * Connect TCP socket
	 * @return
	 */
	MQTTStatus_t TCPconn();

	/***
	 * Execute MQTTprocess to handle receiving and keep alives
	 */
	void MQTTprocess();

	/***
	 * Static functions for requiremts of CoreMQQT TCP connection
	 * Not blocking. Returns 0 if not data available
	 * @param pNetworkContext - network context including link backt to TCP Connection and task
	 * @param pBuffer - buffer to receive into
	 * @param bytesToRecv - length of buffer
	 * @return bytes read
	 */
	static int32_t tcp_recv(NetworkContext_t * pNetworkContext, void * pBuffer, size_t bytesToRecv);

	/***
	 * Static functions for requiremts of CoreMQTT TCP connection. Send data
	 * @param pNetworkContext - network context including link backt to TCP Connection and task
	 * @param pBuffer - buffer to send from
	 * @param bytesToSend - length of buffer
	 * @return
	 */
	static int32_t tcp_send(NetworkContext_t * pNetworkContext, const void * pBuffer, size_t bytesToSend);

	/***
	 * Required by CoreMQTT returns time in ms
	 * @return
	 */
	static uint32_t getCurrentTime();




private:
	NetworkContext_t xNetworkContext;
	TCPConnection tcpConn;
	const char * user;
	const char * passwd;
	const char * id;

	MQTTContext_t xMQTTContext;
	MQTTFixedBuffer_t xBuffer;

	uint8_t netBuffer[BUFFERLEN];

	char topicBuffer[80];
	char payloadBuffer[80];
	static const char * LIFECYCLE_TOPIC;

	const char * target;
	lwesp_port_t port;
	bool ssl;




	//Observer list who will be notified of change
	std::list<MQTTConnObserver *> observers;
};



#endif /* MQTTCONNECTION_H_ */
