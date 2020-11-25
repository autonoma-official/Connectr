
#include "stdio.h"
#include "string.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "gpio_memory_handler.h"
#include "gpio_handler.h"

esp_err_t gpio_load_config(){
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(GPIO_CONFIG, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    size_t entry_size =0;
    err = nvs_get_blob(my_handle, "gpio", NULL, &entry_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND){
        printf("error while get blob\n");
        return err;
    } 
    // Read previously saved blob if available
    if (entry_size > 0) {
        err = nvs_get_blob(my_handle, "gpio", pins, &entry_size);
        if (err != ESP_OK) {
            printf("error while read blob\n");
            return err;
        }

        pinCnt = entry_size/(sizeof(struct Pins));
        printf("Loaded Config. Num of Entries %d\n ", pinCnt);
        for(uint8_t i=0;i<pinCnt;i++){
            printf("id: %s, pin %d, mode %d\n", pins[i].id, pins[i].pin, pins[i].mode);
        }
    }
     // Close
    nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t gpio_save_config(){
    nvs_handle_t my_handle;
    esp_err_t err;

    //save datapoints
    // Open
    err = nvs_open(GPIO_CONFIG, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    size_t blob_size = pinCnt * sizeof(struct Pins);
    err = nvs_set_blob(my_handle, "gpio", pins, blob_size);

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return ESP_OK; 
}

esp_err_t gpio_remove_config(){
    nvs_handle_t my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(GPIO_CONFIG, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;
    err =  nvs_erase_all(my_handle);
    //err =  nvs_erase_key(my_handle, "gpio");
    if (err != ESP_OK) {
            printf("error (%d) while erase blob\n", err);
            return err;
    }
    // Close
    nvs_close(my_handle);

    pinCnt = 0;

    return ESP_OK;

}


