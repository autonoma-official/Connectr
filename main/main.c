#include <stdio.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi_handler.h"
#include "mqtt_handler.h"
#include "i2c/i2c_handler.h"
#include "gpio/gpio_memory_handler.h"
#include "gpio/gpio_handler.h"

#include "i2c/i2c_handler.h"

#include "message_handler.h"

#include "wifi_manager.h"
#include "iot2tangle/iot2tangle.h"
#include "iot2tangle/i2t_mqtt.h"
#include "authentication/auth.h"



static const char *TAG = "MAIN_LOOP";
bool con_flag = false;


//wifi manager for future: https://github.com/tonyp7/esp32-wifi-manager#requirements

/* The examples use WiFi configuration that you can set via project configuration menu
   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

void loop(){
    while (true){
        gpio_loop();
        i2c_loop();
        vTaskDelay( pdMS_TO_TICKS(100) );
    }
    
}

void cb_connection_ok(void *pvParameter){
	ip_event_got_ip_t* param = (ip_event_got_ip_t*)pvParameter;

	/* transform IP to human readable string */
	char str_ip[16];
	esp_ip4addr_ntoa(&param->ip_info.ip, str_ip, IP4ADDR_STRLEN_MAX);
    //init_socket(gw_IP, gw_port, "", "", true);

	ESP_LOGI(TAG, "I have a connection and my IP is %s!", str_ip);
    get_credentials(&autonoma_settings.user, &autonoma_settings.pwd, macAdress);
    init_mqtt_handler();
    i2t_init_mqtt_handler();
    con_flag = true;
    
    
    xTaskCreatePinnedToCore(&loop, "main loop", 8048, NULL, 1, NULL, 0);
}

void cb_disconnected(void *pvParameter){
    printf("wifi disconnected!!!!!!\n");
}





void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());

    //load stored configs
    printf("load config\n");
    gpio_load_config();
    printf("pinCnt is: %d\n", pinCnt);
    init_hw();
    i2c_init();

    


    //init_wifi_handler();

    /* start the wifi manager */
	wifi_manager_start();

	/* register a callback as an example to how you can integrate your code with the wifi manager */
	wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &cb_connection_ok);
    wifi_manager_set_callback(WM_EVENT_STA_DISCONNECTED, &cb_disconnected);


    uint8_t baseMac[6];
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	sprintf(macAdress, "%02x:%02x:%02x:%02x:%02x:%02x", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
    printf("Print MAC Address: %s\n", macAdress);

    
/*

    
    
    init_msg_handler();
    init_msg_handler();
    printf("after init_mqtt\n");


    */
   

    /*while (true) {
        //printf("in while true\n");
        //mqtt_publish(topic, message );
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        //read_sht30();
        //read_values();
        //send_datapoints();
        //f_retval = mjd_sht3x_cmd_get_single_measurement(&sht3x_config, &sht3x_data);
        //ESP_LOGI(TAG, "    RelHum: %7.3f | Temp C: %7.3f | DewPoint C: %7.3f | Temp F: %7.3f", sht3x_data.relative_humidity,
        
        //      sht3x_data.temperature_celsius, sht3x_data.dew_point_celsius, sht3x_data.temperature_fahrenheit);
        
    }*/
    
}