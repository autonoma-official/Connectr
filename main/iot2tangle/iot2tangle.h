#ifndef IOT2TANGLE_H_
#define IOT2TANGLE_H_


char  i2tJSON[1023];
#define gw_IP "139.59.131.235/sensor_data"
#define gw_port 8080

void send_to_gateway(char* sensorMsg);


#endif