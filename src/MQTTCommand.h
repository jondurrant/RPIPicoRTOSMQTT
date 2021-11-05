/*
 * MQTTCommand.h
 * Type used in the command queue for the MQTTTask.
 * Itend to extend to allow selection of QOS on a command
 *
 *  Created on: 30 Oct 2021
 *      Author: jondurrant
 */

#ifndef MQTTCOMMAND_H_
#define MQTTCOMMAND_H_

enum MQTTCommand_e {  Nop, Init, TCPConn, Conn, Pub, Sub, Close, Reconn };

class MQTTCommand {
public:
	MQTTCommand();
	MQTTCommand(MQTTCommand_e cmd);
	virtual ~MQTTCommand();

	MQTTCommand_e getCmd() const;

	void setCmd(MQTTCommand_e cmd) ;



private:
	MQTTCommand_e cmd;


};

#endif /* MQTTCOMMAND_H_ */
