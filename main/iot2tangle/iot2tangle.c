#include "stdio.h"
#include <stdlib.h>
#include "string.h"

#include "iot2tangle.h"
#include "../mqtt_handler.h"
#include "i2t_mqtt.h"




void build_JSON(char* sensorMsg){
    strcpy(i2tJSON, "{\"iot2tangle\":[");
    strcat(i2tJSON, sensorMsg);
    strcat(i2tJSON, "],\"device\":\"");
    strcat(i2tJSON, "DEVICE_ID_1");
    strcat(i2tJSON, "\",\"timestamp\":\"0\"}");
    printf("final i2tJSON: %s\n", i2tJSON);
}

void send_to_gateway(char* sensorMsg){
    build_JSON(sensorMsg);
    i2t_mqtt_publish("iot2tangle", i2tJSON);
    //bool b_socket = socket_sender(gw_IP, gw_port, "", "", "", i2tJSON, 0);
    //printf("after socket con\n");
    //vTaskDelay(20 / portTICK_PERIOD_MS);

}