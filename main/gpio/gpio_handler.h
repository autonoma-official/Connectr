#ifndef GPIO_HANDLER_H_
#define GPIO_HANDLER_H_

#include "stdio.h"

#define MAX_PINS 32

enum pinMode{
    DI,
    DO,
    AI,
    AO,
    None
};


struct Pins {
   char  id[50];
   uint8_t  pin;
   enum pinMode  mode;
   uint16_t  interval; //sending interval in seconds (1-3600)
   
};

uint8_t pinCnt;
struct Pins pins[MAX_PINS];
unsigned long lastTime[MAX_PINS];

void gpio_message_handler(char* topic, char* message);
enum pinMode pinmode_of(char* str_mode);
void gpio_event_handler(char* message);
void init_hw();
void gpio_loop();


#endif