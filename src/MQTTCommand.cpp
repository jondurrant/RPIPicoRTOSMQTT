/*
 * MQTTCommand.cpp
 * Type used in the command queue for the MQTTTask.
 * Itend to extend to allow selection of QOS on a command
 *
 *  Created on: 30 Oct 2021
 *      Author: jondurrant
 */

#include <stdlib.h>
#include "MQTTCommand.h"

MQTTCommand::MQTTCommand(){
	setCmd(Nop);
}

MQTTCommand::MQTTCommand(MQTTCommand_e cmd){//, char * topic , void * payload , unsigned int payloadLen ) {
	setCmd(cmd);
}

MQTTCommand::~MQTTCommand() {

}

MQTTCommand_e MQTTCommand::getCmd() const {
	return cmd;
}

void MQTTCommand::setCmd(MQTTCommand_e cmd) {
	this->cmd = cmd;
}





