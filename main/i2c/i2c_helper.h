#ifndef I2C_HELPER_H_
#define I2C_HELPER_H_

#include "stdio.h"
#include <math.h>
#include "driver/timer.h"

#include "esp_log.h"
#include "driver/i2c.h"


#define I2C_SCL_IO				22	//19               /*!< gpio number for I2C master clock */
#define I2C_SDA_IO				21	//18               /*!< gpio number for I2C master data  */
#define I2C_FREQ_HZ				100000           /*!< I2C master clock frequency */
#define I2C_PORT_NUM			I2C_NUM_0        /*!< I2C port number for master dev */
#define I2C_TX_BUF_DISABLE  	0                /*!< I2C master do not need buffer */
#define I2C_RX_BUF_DISABLE  	0                /*!< I2C master do not need buffer */
#define I2C_MAX_TICKS_TO_WAIT_DEFAULT (1000 / portTICK_PERIOD_MS) /*!< The default 1000ms can be changed. */

// I2C common protocol defines
#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */  



/*uint8_t i2c_entries;
uint8_t addresses[10];
uint16_t commands[20];
uint8_t command_len[10];
uint8_t read_len[10];
char formulas[10][50];*/



void i2c_master_init();
void read_values();
static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, size_t addr_len, uint8_t* i2c_reg, size_t reg_len, uint8_t* data_rd, size_t data_len);
static esp_err_t i2c_master_write_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_wr, size_t size);
#endif