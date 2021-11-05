/*
 * TCPConnectionObserver.h
 * Connection observer which will receive notifications of any connection state
 * changes. By default does nothing
 * Used to inherit from so objects can react to connection becoming active, closed,
 * etc.
 *
 *  Created on: 25 Oct 2021
 *      Author: jondurrant
 */

#ifndef TCPCONNECTIONOBSERVER_H_
#define TCPCONNECTIONOBSERVER_H_

#include "NetConnection.h"

class TCPConnectionObserver {
public:
	TCPConnectionObserver();
	virtual ~TCPConnectionObserver();

	/***
	 * Connection has become active
	 * @param c - NetConnection object within which is a link back to the TCPConnection object
	 */
	virtual void connActive(NetConnection * c);

	/***
	 * Connection has closed
	 * @param c - NetConnection object within which is a link back to the TCPConnection object
	 */
	virtual void connClosed(NetConnection * c);

	/***
	 * data has been sent
	 * @param c - NetConnection object within which is a link back to the TCPConnection object
	 */
	virtual void dataSent(NetConnection * c);

	/***
	 * data is available to read
	 * @param c - NetConnection object within which is a link back to the TCPConnection object
	 */
	virtual void dataRecv(NetConnection * c);

	/***
	 * Attempt to connect has failed
	 * @param c - NetConnection object within which is a link back to the TCPConnection object
	 */
	virtual void connError(NetConnection * c);
};

#endif /* TCPCONNECTIONOBSERVER_H_ */
