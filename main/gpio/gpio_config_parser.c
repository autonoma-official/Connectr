#include "stdio.h"
#include "cJSON.h"
#include "gpio_handler.h"
#include "string.h"

#include "gpio_memory_handler.h"

//returns 0 for success and 1 if parsing failed
uint8_t gpio_parse_config(char* configMsg){
    cJSON *msg = cJSON_Parse(configMsg);
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

    //check if config should be removed
    const cJSON *rm = NULL;
    rm = cJSON_GetObjectItemCaseSensitive(msg, "remove");
    
    if (rm != NULL && rm->valueint == 1){
        printf("rm boolean value is %d\n", rm->valueint);
        printf("Remove GPIO Config\n");
        gpio_remove_config();
        cJSON_Delete(msg);
        return 0;
    }

    const cJSON *jPins = NULL;
    jPins = cJSON_GetObjectItemCaseSensitive(msg, "pins");
    if (jPins != NULL){
        printf("got pin config\n");
        const cJSON *jPin = NULL;
        cJSON_ArrayForEach(jPin, jPins){

            cJSON *gpio_id = cJSON_GetObjectItemCaseSensitive(jPin, "id");
            cJSON *gpio_pin = cJSON_GetObjectItemCaseSensitive(jPin, "pin");
            cJSON *gpio_mode = cJSON_GetObjectItemCaseSensitive(jPin, "mode");
            cJSON *gpio_interval = cJSON_GetObjectItemCaseSensitive(jPin, "interval");

            if (!cJSON_IsString(gpio_id) || !cJSON_IsNumber(gpio_pin) || !cJSON_IsString(gpio_mode) || !cJSON_IsNumber(gpio_interval))
            {
                status = 1;
                printf("One or more JSON Parameters are not valid!\n");
                return status;
            }

            strcpy(pins[pinCnt].id, gpio_id->valuestring);
            pins[pinCnt].pin = gpio_pin->valueint;
            pins[pinCnt].mode = pinmode_of(gpio_mode->valuestring);
            if (pins[pinCnt].mode == None){
                printf("got invalid pin-mode\n");
                return 1;
            }
            pins[pinCnt].interval = gpio_interval->valueint;
            

            printf("ID: %s\n", pins[pinCnt].id);
            printf("PIN: %d\n", pins[pinCnt].pin);
            printf("MODE: %d\n", pins[pinCnt].mode);
            pinCnt++;
        }
    }
    
    cJSON_Delete(msg);
    gpio_save_config();
    return 0;
}