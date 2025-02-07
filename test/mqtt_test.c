#include "MQTTClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADDRESS "tcp://192.168.55.52:1883"
#define CLIENTID "3b98e470-ad93-4305-a500-1b37d343e64a"
#define TOPIC_PULL "pull"
#define TOPIC_PUSH "push"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

MQTTClient_deliveryToken deliveredtoken;

/* 发送消息的回调函数 */
void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

/* 收到消息的回调函数 */
int msgarrvd(void *context, char *topicName, int topicLen,
			 MQTTClient_message *message)
{
	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: %.*s\n", message->payloadlen, (char *)message->payload);
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

/* 连接断开的回调函数 */
void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}

int main(int argc, char *argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;

    /* 创建客户端 */
	if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
								MQTTCLIENT_PERSISTENCE_NONE, NULL)) !=
		MQTTCLIENT_SUCCESS) {
		printf("Failed to create client, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto exit;
	}

    /* 设备回调函数 */
	if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd,
									  delivered)) != MQTTCLIENT_SUCCESS) {
		printf("Failed to set callbacks, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

    /* 连接服务器 */
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
		printf("Failed to connect, return code %d\n", rc);
		rc = EXIT_FAILURE;
		goto destroy_exit;
	}

    /* 订阅主题 */
	printf("Subscribing to topic : %s\nfor client : %s\n using QoS%d\n\n"
		   "Press Q<Enter> to quit\n\n",
		   TOPIC_PULL, CLIENTID, QOS);
	if ((rc = MQTTClient_subscribe(client, TOPIC_PULL, QOS)) !=
		MQTTCLIENT_SUCCESS) {
		printf("Failed to subscribe, return code %d\n", rc);
		rc = EXIT_FAILURE;
	} else {
		int ch;
		do {
			ch = getchar();
		} while (ch != 'Q' && ch != 'q');

		if ((rc = MQTTClient_unsubscribe(client, TOPIC_PULL)) !=
			MQTTCLIENT_SUCCESS) {
			printf("Failed to unsubscribe, return code %d\n", rc);
			rc = EXIT_FAILURE;
		}
	}

    /* 发送消息 */
	pubmsg.payload = PAYLOAD;
	pubmsg.payloadlen = (int)strlen(PAYLOAD);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;
	if ((rc = MQTTClient_publishMessage(client, TOPIC_PUSH, &pubmsg, &token)) !=
		MQTTCLIENT_SUCCESS) {
		printf("Failed to publish message, return code %d\n", rc);
		rc = EXIT_FAILURE;
	} else {
		printf("Waiting for publication of : %s\n"
			   "on topic : %s for client with ClientID: %s\n",
			   PAYLOAD, TOPIC_PUSH, CLIENTID);
		while (deliveredtoken != token) {
			usleep(10000L);
		}
	}

	if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS) {
		printf("Failed to disconnect, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}

destroy_exit:
	MQTTClient_destroy(&client);

exit:
	return rc;
}
