#include "stdio.h"
#include "gpio_handler.h"
#include "string.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gpio_config_parser.h"
#include "iot2tangle.h"
#include "../mqtt_handler.h"

enum pinMode pinmode_of(char* str_mode){
    if (strcmp(str_mode, "DO") == 0){
        return DO;
    }else if (strcmp(str_mode, "DI") == 0){
        return DI;
    }else if (strcmp(str_mode, "AO") == 0){
        return AO;
    }else if (strcmp(str_mode, "AI") == 0){
        return AI;
    }else{
        printf("got nothing vaild\n");
        return None;
    }
    return None;
}

char* str_of(enum pinMode mode){
    switch(mode){
        case DO:
            return "DO";
            break;
        case DI:
            return "DI";
            break;
        case AO:
            return "AO";
            break;
        case AI:
            return "AI";
            break;
        default: 
            return "NO_PIN_MODE";      
    }
}

void build_iot2tangle_msg(uint8_t cfgNr, uint8_t val, char* msg){
    sprintf(msg, "{\"sensor\": \"%s\", \"data\":[{\"%s%d\":%d}]}", pins[cfgNr].id, str_of(pins[cfgNr].mode), pins[cfgNr].pin, val );
    printf("IOT2Tangle MSG: %s\n", msg);
}

esp_err_t gpio_read_h(uint8_t cfgNr){
    printf("in gpio read for cfgNr %d  \n", cfgNr);
    if(pins[cfgNr].mode ==DI){
        printf("read val\n");
        uint8_t val = gpio_get_level(pins[cfgNr].pin);
        printf("build topic\n");
        char topic[200];
        strcpy(topic, macAdress);
        strcat(topic, "/");
        strcat(topic, pins[cfgNr].id);
        //printf("build payload\n");
        char payload[50];
        sprintf(payload, "{\"value\":%d}", val);
        mqtt_publish(topic, payload);

        //build i2t_message
        char i2tMsg[300];
        build_iot2tangle_msg(cfgNr, val, i2tMsg);
        send_to_gateway(i2tMsg);
        //printf("send to gateway\n");

    }
    return 0;
}

esp_err_t gpio_write_h(uint8_t pin, enum pinMode mode, uint8_t value){
    if(mode ==DO){
        if(value !=0 && value != 1){
            printf("Invalid Value for digital pin!\n");
            return ESP_FAIL;
        }
        if(gpio_set_level(pin, value) != ESP_OK){
            printf("Set GPIO Level failed\n");
            return ESP_FAIL;
        }
        return ESP_OK;
    }
    return ESP_FAIL;
}

void gpio_event_handler(char* message){
    cJSON *msg = cJSON_Parse(message);
    uint8_t status = 1;
    const cJSON *entry_b = NULL;

    if (msg == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        return 1;
    }

    cJSON *gpio_id = cJSON_GetObjectItemCaseSensitive(msg, "id");
    cJSON *gpioWrite = cJSON_GetObjectItemCaseSensitive(msg, "write");
    cJSON *read = cJSON_GetObjectItemCaseSensitive(msg, "read"); 

    if(read != NULL){

    }

    if(gpioWrite != NULL){
        //search for id
        for(uint8_t i=0; i<pinCnt; i++){
            printf("compare %s with %s\n",gpio_id->valuestring, pins[i].id );
            if(strcmp(gpio_id->valuestring, pins[i].id)==0){
                printf("set gpio %d to %d\n", pins[i].pin, gpioWrite->valueint);
                if (gpio_write_h(pins[i].pin, pins[i].mode, gpioWrite->valueint) == ESP_OK){
                    char topic[200];
                    strcpy(topic, macAdress);
                    strcat(topic, "/");
                    strcat(topic, pins[i].id);
                    char payload[50];
                    sprintf(payload, "{\"value\":%d}", gpioWrite->valueint);
                    mqtt_publish(topic, payload);
                }
                
            }
        }
        
    }


}

void gpio_message_handler(char* topic, char* message){
    char * token = strtok(topic, "/");
    while( token != NULL ) {
      printf( "Token:  %s\n", token ); //printing each token
      if(strcmp(token, "event")==0){
          printf("In Event\n");
          gpio_event_handler(message);
          return;
      }else if(strcmp(token, "config")==0){
          printf("In config\n");
          gpio_parse_config(message);
          init_hw();
      }
      
      token = strtok(NULL, "/");
   } 
}



void init_hw(){
    printf("GPIO INIT\n");
    for (uint8_t i = 0; i<pinCnt; i++){
        gpio_reset_pin(pins[i].pin);
        if (pins[i].mode == DO){
            if(gpio_set_direction(pins[i].pin, GPIO_MODE_OUTPUT) != ESP_OK){
                printf("gpio initializing fialed!!!!!\n");
            }
        }else if (pins[i].mode == DI){
            gpio_set_direction(pins[i].pin, GPIO_MODE_INPUT);
        }else{
            printf("This pinmode is not supported for now\n");
        }
    }
}

void gpio_loop(){

    unsigned long currentMillis = xTaskGetTickCount() * portTICK_PERIOD_MS;
    for(uint8_t i = 0; i<pinCnt; i++){
        if (pins[i].mode == DI){
            if(currentMillis >= lastTime[i]+pins[i].interval*1000){
                printf("before gpio read\n");
                gpio_read_h(i);
                lastTime[i] = currentMillis;
                printf("after gpio read\n");
            }
        }
    }
}