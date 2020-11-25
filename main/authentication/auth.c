#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "../mqtt_handler.h"

static const char *TAG = "GET_CREDS";
esp_http_client_handle_t client;


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // Write out data
                // printf("%.*s", evt->data_len, (char*)evt->data);
            }
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
	}
    return ESP_OK;
}

void parse_authentication_response(char* buffer ){
     cJSON *msg = cJSON_Parse(buffer);

    if (msg == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        ESP_LOGE(TAG, "Invalid JSON Message");
        return;
    }
    const cJSON *dID = NULL;
    const cJSON *duser = NULL;
    const cJSON *dpw = NULL;
    dID = cJSON_GetObjectItemCaseSensitive(msg, "clientid");
    duser = cJSON_GetObjectItemCaseSensitive(msg, "user");
    dpw = cJSON_GetObjectItemCaseSensitive(msg, "password");

    if (!cJSON_IsString(dID) || !cJSON_IsString(duser) || !cJSON_IsString(dpw))
    {
        ESP_LOGE(TAG, "Invalid JSON Payload");
    }
    strcpy(m_client_id, dID->valuestring);
    strcpy(m_username, duser->valuestring);
    strcpy(m_password, dpw->valuestring);

}


bool get_credentials(char* username, char* password, char* mac)
{
    printf("in get creds\n");

    esp_err_t err; 
    char local_response_buffer[200] = {0};
    char m_url[300];
    sprintf(m_url, "https://app.autonoma.cloud/device/register?username=%s&password=%s&mac=%s", username, password, mac);
    printf("url: %s", m_url);
    
    // POST	
	esp_http_client_config_t config = {
	    .url = m_url, 
	    .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
	};

    client = esp_http_client_init(&config);

	err = esp_http_client_perform(client);	// Check HTTP Endpoint		
    if (err == ESP_OK)
	{
		ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
        size_t content_length = esp_http_client_get_content_length(client);
        char buffer[400];
        size_t read_len = esp_http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        parse_authentication_response(buffer);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    return ESP_OK;
	
}





