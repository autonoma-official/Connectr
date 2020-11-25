#ifndef I2C_HANDLER_H_
#define I2C_HANDLER_H_

#include "stdio.h"


#define MAX_DATAPOINTS 5
#define MAX_READ_CMDs 5
#define MAX_CMD_LEN 64

//for string to hex conversion
static const long hextable[] = {
   [0 ... 255] = -1, // bit aligned access into this table is considerably
   ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
   ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
   ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};


unsigned long i2c_read_time;
struct DataPoints {
   char  id[50];
   uint8_t  byte_cnt;
   uint8_t  cmd_nr[20];
   uint8_t  byte_nr[20];
   char formula[50];
};

struct ReadCommands {
   uint8_t  i2c_adr;
   uint8_t  cmd[MAX_CMD_LEN];
   uint8_t  cmd_len;
   uint8_t  read_len;
   uint8_t  read_dat[10];
};

uint8_t nr_datapoints;
uint8_t nr_readcommands;
struct DataPoints dataPoints[MAX_DATAPOINTS];
struct ReadCommands readCommands[MAX_READ_CMDs];
unsigned long i2c_lastTime[MAX_DATAPOINTS];

void i2c_init();
void i2c_loop();
void i2c_message_handler(char* topic, char* message);



#endif