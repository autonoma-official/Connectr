#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include "stdio.h"




int8_t parse_mqtt_message(char* topic, int topic_len, char* message, int msg_len);

#endif