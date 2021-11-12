/*
 * TCPConnection.h
 *
 * Plain TCP Socket connection class, implemted to use lwesp over ESP-AT
 * Adapted to fit to the CoreMQTT library requirements
 * Intesion to extend to support SSL
 *
 *  Created on: 25 Oct 2021
 *      Author: jondurrant
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "lwesp/lwesp.h"
#include "NetConnection.h"
#include "TCPConnectionObserver.h"
#include <list>
#include <semphr.h>

class TCPConnection : public NetConnection{
public:
	TCPConnection();
	virtual ~TCPConnection();

	/***
	 * Connect to host
	 * @param target - host name or ip address
	 * @param port - port number
	 * @return lwespOK if succeeds
	 */
	lwespr_t connect(const char * target, lwesp_port_t  port);

	/***
	 * Reconnect to previously connected target
	 * @return lwespOK if succeeds
	 */
	lwespr_t reconnect();

	/***
	 * Close Connection
	 * @return
	 */
	lwespr_t close();

	/***
	 * Send data down socket
	 * @param data = pointer to data - raw memory
	 * @param size = bytes of data
	 * @return lwespOK if succeed
	 */
	lwespr_t send(const char * data, size_t size);

	/***
	 * Received data from socket. Non blocking
	 * @param pBuffer = buffer to write to
	 * @param bytesToRecv = maximum size of bugger
	 * @return number of bytes read. Zero if none available
	 */
	int32_t recv(char * pBuffer,size_t bytesToRecv );

	/***
	 * Attach an observer which will be notified of events
	 * @param observer
	 */
	void attach(TCPConnectionObserver *observer);

	/***
	 * Detach observer
	 * @param observer
	 */
	void detach(TCPConnectionObserver *observer);


	/***
	 * Private call back function to interface to lwesp
	 * @param evt
	 * @return
	 */
	static lwespr_t connCallbackFunc(lwesp_evt_t* evt);

protected:
	/***
	 * Internal interface to lwesp library/
	 * @param evt
	 * @return
	 */
	lwespr_t connEvt(lwesp_evt_t* evt);

private:
	// Connection object used by lwesp
	lwesp_conn_p conn;

	//Observer list who will be notified of change
	std::list<TCPConnectionObserver *> observers;

	//Receive buffer and handlers for converting between interface and bugger
	lwesp_pbuf_p recBuf = NULL;
	size_t bufCount = 0;
	size_t bufLen = 0;

	//Host and port address cached for reconnect
	const char * target;
	lwesp_port_t  port;

	SemaphoreHandle_t  xSemRecBuf = NULL;

};

#endif /* TCPCONNECTION_H_ */
