/*
 * NetConnection.h
 *
 * Abstract class for nentwork connection.  Intented to allow other connection
 * protocols than just the plain TCP socket.
 *
 *  Created on: 25 Oct 2021
 *      Author: jondurrant
 */

#ifndef NETCONNECTION_H_
#define NETCONNECTION_H_

#include "lwesp/lwesp.h"

class NetConnection {
public:
	NetConnection();
	virtual ~NetConnection();

	/***
	 * Connect to host
	 * @param target - host name of ip address as string
	 * @param port - port to conenct to
	 * @return lwespOK if succeed
	 */
	virtual lwespr_t connect(const char * target, lwesp_port_t  port)=0;

	/***
	 * Reconnect to previous target
	 * @return
	 */
	virtual	lwespr_t reconnect()=0;

	/***
	 * Close connection
	 * @return
	 */
	virtual	lwespr_t close()=0;

	/***
	 * Send data down socket
	 * @param data - pointer to raw memory of the data
	 * @param size - number of bytes of data pointed to
	 * @return
	 */
	virtual lwespr_t send(const char * data, size_t size) = 0;

	/***
	 * Received data from netwok. If no data available will return length of zero
	 * @param pBuffer = buffer to right to
	 * @param bytesToRecv = size of bugger in bytes
	 * @return number of bytes read, zero if nothing waiting
	 */
	virtual int32_t recv(char * pBuffer,size_t bytesToRecv )=0;


};

#endif /* NETCONNECTION_H_ */
