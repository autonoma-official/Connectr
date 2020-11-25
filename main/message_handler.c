#include "message_handler.h"
#include "i2c_handler.h"
#include "cJSON.h"
#include "tinyexpr.h"
#include "mqtt_handler.h"

#include "stdio.h"
#include <stdlib.h>
#include "string.h"

#include "gpio_handler.h"
#include "gpio_config_parser.h"

#include "i2c_handler.h"
#include "i2c_config_parser.h"





int8_t parse_mqtt_message(char* topic, int topic_len, char* message, int msg_len){

    //get a copy of the topic string, because it gets modified by strtok
    char* sTopic = malloc(sizeof(char) * (topic_len +1));
    strncpy(sTopic, topic, topic_len);
    sTopic[topic_len] = '\0';

    printf("in parse mqtt message \n");
    //spit topic string at "/"
    char * token = strtok(topic, "/");
    while( token != NULL ) {
      printf( "Token:  %s\n", token ); //printing each token
      if(strcmp(token, "online") == 0){
          //ignore online-status retention
          printf("got online topic\n");
          break;
      }
      else if(strcmp(token, "gpio") ==0){
          printf("go to gpio_message handler\n");
          gpio_message_handler(sTopic, message);
          break;
      }else if(strcmp(token, "i2c") ==0){
          printf("go to i2c_message handler\n");
          i2c_message_handler(sTopic, message);
          break;
      }
      token = strtok(NULL, "/" );
   }
   printf("after while token\n");

   free(sTopic);

   return 0;

}


