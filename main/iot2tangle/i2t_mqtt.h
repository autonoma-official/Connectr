#ifndef I2T_MQTT_HANDLER_H_
#define I2T_MQTT_HANDLER_H_



char macAdress[18];

void i2t_init_mqtt_handler();
int i2t_mqtt_publish(const char* topic, const char* data);

#endif