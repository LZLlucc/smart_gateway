#ifndef APP_MQTT_H_
#define APP_MQTT_H_

#define ADDRESS "tcp://192.168.55.52:1883"
#define CLIENTID "3b98e470-ad93-4305-a500-1b37d343e64a"
#define TOPIC_PULL "pull"
#define TOPIC_PUSH "push"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

typedef int (*recv_callback)(char *datas, int datas_len);

int app_mqtt_init();
int app_mqtt_publish(char *datas, int datas_len);
void app_mqtt_register_recv_callback(recv_callback callback);
void app_mqtt_close();

#endif /* APP_MQTT_H_ */