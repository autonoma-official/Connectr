#include "mqtt_handler.h"
#include "mqtt_client.h"
#include "config.h"

#include "message_handler.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include "esp_log.h"



static const char *TAG = "MQTTS_EXAMPLE";

esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    //esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            char subTopic[25];
            strcpy(subTopic, macAdress);
            strcat(subTopic, "/#");

            msg_id = esp_mqtt_client_subscribe(client, subTopic, 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            //msg_id = esp_mqtt_client_publish(client, "24:0a:c4:97:71:2c/online", "0", 0, 0, 0);
            //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            char onlineTopic[25];
            strcpy(onlineTopic, macAdress);
            strcat(onlineTopic, "/online");
            msg_id = esp_mqtt_client_publish(client, onlineTopic, "1", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            //printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            //printf("DATA=%.*s\r\n", event->data_len, event->data);

            char* sTopic = malloc(sizeof(char) * (event->topic_len)+1);
            strncpy(sTopic, event->topic, event->topic_len);
            sTopic[event->topic_len] = '\0';
            char* sMessage = malloc(sizeof(char) * event->data_len);
            strncpy(sMessage, event->data, event->data_len);

            parse_mqtt_message(sTopic, event->topic_len, sMessage, event->data_len);
            free(sTopic);
            free(sMessage);

            

            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_ESP_TLS) {
                ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
                ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
                ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
            } else {
                ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void init_mqtt_handler(){
    char lwtTopic[25];
    strcpy(lwtTopic, macAdress);
    strcat(lwtTopic, "/online");
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = broker_url,
        .client_id = m_client_id,
        .username = m_username,
        .password = m_password,
        .event_handle = mqtt_event_handler,
        .lwt_topic = lwtTopic,
        .lwt_msg = "0"
        //.cert_pem = (const char *)mqtt_eclipse_org_pem_start,
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}



int mqtt_publish(const char* topic, const char* data){
    int msg_id;
    msg_id = esp_mqtt_client_publish(client, topic, data, 0, 0, 0);
    return msg_id;  
}



