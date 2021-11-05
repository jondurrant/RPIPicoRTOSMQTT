/***
 * Debug printf utility.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include "MQTTDebug.h"

#ifndef MQTTDEBUG

void dbg(const char *fmt, ...) {
	//nop
}

#endif


#ifdef MQTTDEBUG
void dbg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

#endif
