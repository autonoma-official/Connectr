#include "i2c_helper.h"
#include <stdio.h>
#include "../message_handler.h"
#include <math.h>
#include "driver/timer.h"

#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c_handler.h"




static const char TAG[] = "i2c_helper";


static void _delay_millisec(uint32_t millisec) {
    if (millisec > 0) {
        ets_delay_us(millisec * 1000);
    }
}

static esp_err_t _send_cmd(uint8_t i2c_addr, size_t addr_len, uint8_t* i2c_reg, size_t reg_len) {
    ESP_LOGD(TAG, "%s()", __FUNCTION__);

    esp_err_t f_retval = ESP_OK;
    i2c_cmd_handle_t handle;

    ESP_LOGD(TAG, "Send Command");

    handle = i2c_cmd_link_create();

    f_retval = i2c_master_start(handle);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request i2c_master_start() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }
    f_retval = i2c_master_write_byte(handle, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request i2c_master_write_byte() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }
    uint8_t i = 0;
    for(i=0; i<reg_len; i++){
        f_retval = i2c_master_write_byte(handle, i2c_reg[i], true);
        if (f_retval != ESP_OK) {
            ESP_LOGE(TAG, "%s(). ABORT. Send request i2c_master_write_byte() err %i (%s)", __FUNCTION__, f_retval,
                    esp_err_to_name(f_retval));
            i2c_cmd_link_delete(handle);
            // GOTO
            goto cleanup;
        }
    }
    
    f_retval = i2c_master_stop(handle);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Receive response i2c_master_stop() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    f_retval = i2c_master_cmd_begin(I2C_PORT_NUM, handle, I2C_MAX_TICKS_TO_WAIT_DEFAULT);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Receive response i2c_master_cmd_begin() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    i2c_cmd_link_delete(handle);

    _delay_millisec(1);

    // LABEL
    cleanup: ;

    return f_retval;
}


static esp_err_t i2c_master_read_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, size_t addr_len, uint8_t* i2c_reg, size_t reg_len, uint8_t* data_rd, size_t data_len)
{
    printf("register: %d\n", i2c_reg[0]);
    printf("register: %d\n", i2c_reg[1]);
    esp_err_t f_retval = ESP_OK;
    f_retval = _send_cmd(i2c_addr, addr_len, i2c_reg, reg_len);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT _send_cmd() err %i (%s)", __FUNCTION__, f_retval, esp_err_to_name(f_retval));
        // GOTO
        goto cleanup;
    }

    uint32_t delay = 85;
    ESP_LOGD(TAG, "%s(). Computed delay (ms): %u", __FUNCTION__, delay);
    _delay_millisec(delay);

    //Read Data
    i2c_cmd_handle_t handle;
    uint8_t rx_buf[6] =
                { 0 };

    handle = i2c_cmd_link_create();

    f_retval = i2c_master_start(handle);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request i2c_master_start() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }
    f_retval = i2c_master_write_byte(handle, (i2c_addr << 1) | I2C_MASTER_READ, true);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Send request i2c_master_write_byte() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    printf("read data len: %d\n", data_len);
    f_retval = i2c_master_read(handle, data_rd, data_len, I2C_MASTER_LAST_NACK);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Receive response i2c_master_read() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    f_retval = i2c_master_stop(handle);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Receive response i2c_master_stop() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    f_retval = i2c_master_cmd_begin(I2C_PORT_NUM, handle, I2C_MAX_TICKS_TO_WAIT_DEFAULT);
    if (f_retval != ESP_OK) {
        ESP_LOGE(TAG, "%s(). ABORT. Receive response i2c_master_cmd_begin() err %i (%s)", __FUNCTION__, f_retval,
                esp_err_to_name(f_retval));
        i2c_cmd_link_delete(handle);
        // GOTO
        goto cleanup;
    }

    i2c_cmd_link_delete(handle);

    for (uint32_t j = 0; j < data_len; j++) {
        ESP_LOGD(TAG, "%s(). rx_buf[%u]: %u", __FUNCTION__, j, rx_buf[j]);
    }

   

    // LABEL
    cleanup: ;

    return f_retval;

}
static esp_err_t i2c_master_write_slave_reg(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be written
    i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, i2c_reg, ACK_CHECK_EN);
    // write the data
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
} 

/**
 * @brief i2c master initialization
 */
void i2c_master_init()
{
    int i2c_master_port = I2C_PORT_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);

    i2c_driver_install(i2c_master_port, conf.mode,
    		I2C_RX_BUF_DISABLE, I2C_TX_BUF_DISABLE, 0);

    _delay_millisec(2);
}

void read_values(){
    for(uint8_t i=0; i< nr_readcommands;i++){
        printf("read i2c address %d\n",readCommands[i].i2c_adr );
        uint8_t len = readCommands[i].cmd_len;
        i2c_master_read_slave_reg(I2C_PORT_NUM, readCommands[i].i2c_adr, 1, readCommands[i].cmd, len, readCommands[i].read_dat, readCommands[i].read_len);
        //float temperature_celsius = -45.0 + 175.0 * (readCommands[i].read_dat[0] * 256.0 + readCommands[i].read_dat[1]) / 65535.0;
        //printf("sensor value: %f\n", temperature_celsius);
    }    
}



