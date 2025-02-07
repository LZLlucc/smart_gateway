#include "MQTTClient.h"
#include "app_common.h"
#include "app_mqtt.h"
#include "log/log.h"

#define ADDRESS "tcp://192.168.55.52:1883"
#define CLIENTID "3b98e470-ad93-4305-a500-1b37d343e64a"
#define TOPIC_PULL "pull"
#define TOPIC_PUSH "push"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

static MQTTClient_deliveryToken deliveredtoken;
static MQTTClient client;
static MQTTClient_connectOptions conn_opts =
	MQTTClient_connectOptions_initializer;
static MQTTClient_message pubmsg = MQTTClient_message_initializer;
static recv_callback recv_cb = NULL;

/* 发送消息的回调函数 */
static void delivered(void *context, MQTTClient_deliveryToken dt)
{
	log_debug("Message publish succ");
	deliveredtoken = dt;
}

/* 收到消息的回调函数 */
static int msgarrvd(void *context, char *topicName, int topicLen,
					MQTTClient_message *message)
{
	int rs = 0;
	if (recv_cb) {
		rs = recv_cb((char *)message->payload, message->payloadlen) 
                                                                == 0 ? 1 : 0;
	}

	/* 释放消息资源 */
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);

	/* 返回消息处理结果 1 : 成功 0：失败 */
	return rs;
}

/* 连接断开的回调函数 */
void connlost(void *context, char *cause)
{
	log_error("Connection lost, cause: %s", cause);
}

int app_mqtt_init()
{
	int rs = -1;

	/* 1.创建客户端 */
	rs = MQTTClient_create(&client, ADDRESS, CLIENTID,
						   MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rs != MQTTCLIENT_SUCCESS) {
		log_error("MQTTClient creat failed: %d", rs);
		return -1;
	}

	/* 2.设置回调函数 */
	rs = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if (rs != MQTTCLIENT_SUCCESS) {
		log_error("MQTTClient set callback failed: %d", rs);
		return -1;
	}

	/* 3.连接服务器 */
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	rs = MQTTClient_connect(client, &conn_opts);
	if (rs != MQTTCLIENT_SUCCESS) {
		log_error("MQTTClient connect failed: %d", rs);
		return -1;
	}

	/* 4.订阅主题 */
	rs = MQTTClient_subscribe(client, TOPIC_PULL, QOS);
	if (rs != MQTTCLIENT_SUCCESS) {
		log_error("MQTTClient connect failed: %d", rs);
		return -1;
	}

	return 0;
}

/* 发布信息 */
int app_mqtt_publish(char *datas, int datas_len)
{
	pubmsg.payload = datas;
	pubmsg.payloadlen = datas_len;
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	deliveredtoken = 0;
	if (MQTTClient_publishMessage(client, TOPIC_PUSH, &pubmsg, NULL) !=
		MQTTCLIENT_SUCCESS) {
		log_error("Failed to publish message");
		return -1;
	}

	return 0;
}

/* 注册接收的回调函数 */
void app_mqtt_register_recv_callback(recv_callback callback)
{
	recv_cb = callback;
}

void app_mqtt_close()
{
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
}
