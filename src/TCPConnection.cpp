/*
 * TCPConnection.cpp
 *
 * TCP Socket connection using the lwesp library.
 *
 *  Created on: 25 Oct 2021
 *      Author: jondurrant
 */

#include "TCPConnection.h"
#include "MQTTDebug.h"
#include <stdlib.h>

TCPConnection::TCPConnection(){
}


/***
 * Connect to host
 * @param target - host name or ip address
 * @param port - port number
 * @return lwespOK if succeeds
 */
lwespr_t TCPConnection::connect(const char * target, lwesp_port_t  port) {
	lwespr_t res;

	this->target = target;
	this->port = port;

	dbg("TCPConnection::connect(%s, %d)", target, port);

	res = lwesp_conn_start(&conn, LWESP_CONN_TYPE_TCP,
			target, port, this, TCPConnection::connCallbackFunc, 0);

	if (res != lwespOK) {
		dbg("Cannot connect\r\n");
	}
	return res;
}

/***
* Reconnect to previously connected target
* @return lwespOK if succeeds
*/
lwespr_t TCPConnection::reconnect(){
	connect(this->target, this->port);
	return lwespOK;

}

/***
* Close Connection
* @return
*/
lwespr_t TCPConnection::close(){
	return lwesp_conn_close(conn, 0);
}

TCPConnection::~TCPConnection() {
	close();
}

/***
* Private call back function to interface to lwesp
* @param evt
* @return
*/
lwespr_t  TCPConnection::connCallbackFunc(lwesp_evt_t* evt){
	lwesp_conn_p conn = lwesp_conn_get_from_evt(evt);
	TCPConnection *tc = NULL;
	if (lwesp_evt_get_type(evt) != LWESP_EVT_CONN_ERROR){
		tc = (TCPConnection *)lwesp_conn_get_arg(conn);
	} else {
		tc = (TCPConnection *)lwesp_evt_conn_error_get_arg(evt);
	}
	if (tc != NULL){
		tc->connEvt(evt);
	} else {
		dbg("TCPConnection missing\n");
	}
	return lwespOK;
}

/***
* Internal interface to lwesp library/
* @param evt
* @return
*/
lwespr_t TCPConnection::connEvt(lwesp_evt_t *evt) {
	switch (lwesp_evt_get_type(evt)) {
	case LWESP_EVT_CONN_ACTIVE:{
		std::list<TCPConnectionObserver *>::iterator iterator = observers.begin();
		while (iterator != observers.end()) {
		  (*iterator)->connActive(this);
		  ++iterator;
		}
		break;
	}
	case LWESP_EVT_CONN_CLOSE:{
			std::list<TCPConnectionObserver *>::iterator iterator = observers.begin();
			while (iterator != observers.end()) {
			  (*iterator)->connClosed(this);
			  ++iterator;
			}
		break;
	}
	case LWESP_EVT_CONN_SEND: { /* Data send event */
			std::list<TCPConnectionObserver *>::iterator iterator = observers.begin();
			while (iterator != observers.end()) {
			  (*iterator)->dataSent(this);
			  ++iterator;
			}
		break;
	}
	case LWESP_EVT_CONN_RECV: { /* Data received from remote side */
			recBuf = lwesp_evt_conn_recv_get_buff(evt);
			bufLen = lwesp_pbuf_length(recBuf, 1);
			std::list<TCPConnectionObserver *>::iterator iterator = observers.begin();
			while (iterator != observers.end()) {
			  (*iterator)->dataRecv(this);
			  ++iterator;
			}
		break;
	}

	case LWESP_EVT_CONN_ERROR: { // Error connecting to server
			std::list<TCPConnectionObserver *>::iterator iterator = observers.begin();
			while (iterator != observers.end()) {
			  (*iterator)->connError(this);
			  ++iterator;
			}
		break;
	}
	default:
		break;
	}
	return lwespOK;
}

/***
* Send data down socket
* @param data = pointer to data - raw memory
* @param size = bytes of data
* @return lwespOK if succeed
*/
lwespr_t TCPConnection::send(const char * data, size_t size){
	/***DEbug
	printf("Sending Data(%d):%.*s\n",size, size, data);
	for (int i=0; i < size; i++){
		printf("%x.",(uint8_t)data[i]);
	}
	printf("\n");
	***/
	return lwesp_conn_send(conn, data, size, NULL, 0);

}

/***
* Received data from socket. Non blocking
* @param pBuffer = buffer to write to
* @param bytesToRecv = maximum size of bugger
* @return number of bytes read. Zero if none available
*/
int32_t TCPConnection::recv(char * pBuffer,size_t bytesToRecv ){
	if (recBuf == NULL){
		return 0;
	}
	size_t len;
	size_t pos = 0;
	size_t dStart =0;
	size_t dLen = 0;
	void* data;

	len = lwesp_pbuf_length(recBuf, 1);
	dLen = lwesp_pbuf_copy(recBuf, pBuffer, bytesToRecv, bufCount);
	bufCount += dLen;

	//Completed all data so tell lwesp to free up the buffer
	if (len == bufCount){
		lwesp_conn_recved(conn, recBuf);
		bufCount = 0;
		recBuf = NULL;
	}
	return dLen;
}


/***
 * Attach an observer which will be notified of events
 * @param observer
 */
void TCPConnection::attach(TCPConnectionObserver *observer){
	observers.push_back(observer);
}

/***
 * Detach observer
 * @param observer
 */
void TCPConnection::detach(TCPConnectionObserver *observer){
	observers.remove(observer);
}





