#ifndef WIFI_HANDLER_H_
#define WIFI_HANDLER_H_

#define EXAMPLE_ESP_WIFI_SSID      "WieKannManNurSoDummSein"
#define EXAMPLE_ESP_WIFI_PASS      "steveAlter"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5



/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void init_wifi_handler();

#endif