#ifndef MQTT_HANDLER_H_
#define MQTT_HANDLER_H_

#define broker_url  "mqtts://mqtt.autonoma.cloud:8883"
#define broker_port "8883"
char macAdress[18];

char m_client_id[32];
char m_username[32];
char m_password[32];

void init_mqtt_handler();
int mqtt_publish(const char* topic, const char* data);

#endif