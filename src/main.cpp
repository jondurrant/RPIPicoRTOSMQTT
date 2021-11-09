#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include <array>
#include "lwesp/lwesp.h"
#include "MQTTConnTask.h"
#include "MQTTRouterPing.h"
#include "MQTTDebug.h"


//Local include file not in git repository, defining the credentials
#include "Credentials.env"
#ifndef SID
#define SID "sid"
#define PASSWD "passwd"
#endif
#ifndef MQTTHOST
#define MQTTHOST "MQTTSERVER"
#define MQTTPORT 1883
#define MQTTUSER "USER"
#define MQTTPASSWD "PASSWD"
#endif






//MQTTConnection mqttConn;
MQTTConnTask mqttTask;
MQTTRouterPing mqttRouter;


char mqttTarget[] = MQTTHOST;
char mqttUser[] = MQTTUSER;
char mqttPwd[] = MQTTPASSWD;
lwesp_port_t mqttPort = MQTTPORT;



void startMQTT(){
	printf("Starting MQTT\n");
	dbg("TEST DBG");

	mqttTask.credentials(mqttUser, mqttPwd);
	mqttTask.setReconnect(true);
	mqttRouter.init(mqttTask.getId());
	mqttTask.setRouter(&mqttRouter);

	mqttTask.start();
	mqttTask.connect(mqttTarget, mqttPort);
}






lwespr_t lwesp_callback_func(struct lwesp_evt *evt){
	printf("Init Callback\n");
	return lwespOK;

}

void ip2str(char *str, lwesp_ip_t *ip){
	sprintf(str, "%d:%d:%d:%d",
		(int)ip->addr.ip4.addr[0], (int)ip->addr.ip4.addr[1],
		(int)ip->addr.ip4.addr[2], (int)ip->addr.ip4.addr[3]);
}


void
init_thread(void* pvParameters) {
	lwespr_t eres;
	char ipStr[16];

    /* Initialize ESP with default callback function */
    printf("Initializing LwESP\r\n");
    if (lwesp_init(lwesp_callback_func, 1) != lwespOK) {
        printf("Cannot initialize LwESP!\r\n");

    } else {
        printf("LwESP initialized!\r\n");


        if (lwesp_sta_has_ip()) {
        	printf("Already connected\r\n");
        	lwesp_ip_t ip;
			uint8_t is_dhcp;

			lwesp_sta_copy_ip(&ip, NULL, NULL, &is_dhcp);
			ip2str(ipStr, &ip);
			printf("Station IP address: %s\r\n", ipStr);
			printf("; Is DHCP: %d\r\n", (int)is_dhcp);


        } else {
        	char sid[] = SID;
        	char passwd[] = PASSWD;
        	if (lwesp_sta_join(sid, passwd, NULL, NULL, NULL, 1) == lwespOK) {
				 lwesp_ip_t ip;
				 uint8_t is_dhcp;

				 printf("Connected to %s network!\r\n", sid);
				 lwesp_sta_copy_ip(&ip, NULL, NULL, &is_dhcp);
				 ip2str(ipStr, &ip);
				 printf("Station IP address: %s\r\n", ipStr);
				 printf("; Is DHCP: %d\r\n", (int)is_dhcp);

			 } else {
				 printf("Connection error: %d\r\n", (int)eres);
				 return;
			 }
        }
        startMQTT();
    }
    for (;;){
    	vTaskDelay(5000);

    }

}

int main() {

	stdio_init_all();

	sleep_ms(3000);
	printf("******GO******\n");


    BaseType_t atReturned;
	TaskHandle_t atHandle = NULL;
	/* Create the task, storing the handle. */
	atReturned = xTaskCreate(
					init_thread,       /* Function that implements the task. */
					"Init task",   /* Text name for the task. */
					512,             /* Stack size in words, not bytes. */
					( void * ) 1,    /* Parameter passed into the task. */
					tskIDLE_PRIORITY,/* Priority at which the task is created. */
					&atHandle );


    vTaskStartScheduler();
    while(1)
    {
        configASSERT(0);    /* We should never get here */
    }

}
