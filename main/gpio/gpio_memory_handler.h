#ifndef GPIO_MEMORY_HANDLER_H_
#define GPIO_MEMORY_HANDLER_H_

#include "nvs.h"


#define GPIO_CONFIG  "gpio"


esp_err_t gpio_load_config();
esp_err_t gpio_save_config();
esp_err_t gpio_remove_config();

#endif