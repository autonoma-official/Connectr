
#include "stdio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tinyexpr.h"

#include "i2c_config_parser.h"
#include "i2c_handler.h"
#include "i2c_helper.h"
#include "../mqtt_handler.h"
#include "iot2tangle.h"

uint16_t i2c_interval = 30;
uint16_t i2c_read_interval = 5;

double calc_result(uint8_t* data, uint8_t data_len, char* formula){
    //double a,b,c,d,e,f;
    double variables[6];
    te_variable vars[] = {{"a",&variables[0]}, {"b", &variables[1]}, {"c", &variables[2]}, {"d", &variables[3]}, {"e", &variables[4]}, {"f", &variables[5]}};
    int err;

    for(uint8_t i=0;i<data_len;i++){
        variables[i] = (double)data[i];
    }
    /* Compile the expression with variables. */
    te_expr *expr = te_compile(formula, vars, 6, &err);
    if (expr){
        const double res = te_eval(expr);
        printf("calculation result: %f\n", res);
        return res;
    } else {
        printf("Parse error at %d\n", err);
        return -1;
    }

}

void i2c_build_iot2tangle_msg(uint8_t cfgNr, uint8_t val, char* msg){
    sprintf(msg, "{\"sensor\": \"%s\", \"data\":[{\"%s\":%d}]}", dataPoints[cfgNr].id, "I2C", val );
    printf("IOT2Tangle MSG: %s\n", msg);
}



void i2c_message_handler(char* topic, char* message){
    char * token = strtok(topic, "/");
    while( token != NULL ) {
      printf( "Token:  %s\n", token ); //printing each token
      if(strcmp(token, "event")==0){
          //gpio_event_handler(message);
          return;
      }else if(strcmp(token, "config")==0){
          i2c_parse_config(message);
      }
      
      token = strtok(NULL, "/");
   } 
}




void i2c_init(){
    nr_datapoints = 0;
    nr_readcommands = 0;  
    i2c_master_init();
}

void i2c_loop(){

    unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;
    //read I2C Values
    if(currentMillis >= i2c_read_time +i2c_read_interval*1000){
        read_values();
        i2c_read_time = currentMillis;

    }

    for(uint8_t i=0; i<nr_datapoints;i++){
        if(currentMillis >= i2c_lastTime[i]+i2c_interval*1000){
            //get needed i2c data
            uint8_t data[dataPoints[i].byte_cnt];
            for(uint8_t j=0; j<dataPoints[i].byte_cnt;j++){
                data[j]=readCommands[(dataPoints[i].cmd_nr[j])].read_dat[(dataPoints[i].byte_nr[j])];
            }
            
            //calculate value
            double value = calc_result(data, dataPoints[i].byte_cnt, dataPoints[i].formula);
            //generate topic
            char topic[100];
            strcpy(topic,macAdress);
            strcat(topic, "/");
            strcat(topic, dataPoints[i].id);
            //generate value as string
            char value_str[30];
            sprintf(value_str, "{\"value\":%lf}", value);
            printf("send mqtt topic: %s, payload: %s", topic, value_str);
        
            mqtt_publish(topic, value_str);

            //build i2t_message
            char i2tMsg[300];
            i2c_build_iot2tangle_msg(i, value, i2tMsg);
            send_to_gateway(i2tMsg);
            printf("send to gateway\n");
            i2c_lastTime[i] = currentMillis;
        }
    }

}