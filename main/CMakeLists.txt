idf_component_register(
    SRCS "main.c" "message_handler.c" "mqtt_handler.c" "wifi_handler.c" "helper/tinyexpr.c" "gpio/gpio_config_parser.c" "gpio/gpio_handler.c" "gpio/gpio_memory_handler.c" "iot2tangle/iot2tangle.c" "iot2tangle/i2t_mqtt.c" "i2c/i2c_config_parser.c" "i2c/i2c_handler.c" "i2c/i2c_helper.c" "authentication/auth.c"
    INCLUDE_DIRS "gpio" "helper" "i2c" "iot2tangle" "authentication"
)
