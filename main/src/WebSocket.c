#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"

static const char *TAG = "WEBSOCKET_CLIENT";

// Xử lý sự kiện WebSocket
static void websocket_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WebSocket connected");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WebSocket disconnected");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "Received data of length %d", data->data_len);
        ESP_LOGI(TAG, "Data: %.*s", data->data_len, (char *)data->data_ptr);
        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGE(TAG, "WebSocket error occurred");
        break;
    }
}

char *create_cjson()
{
    uint16_t data = 10;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", "ESP32");
    cJSON_AddNumberToObject(root, "distance", data);

    char *json_string = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return json_string;
}

void websocket_client_task(void *pvParameters)
{
    const esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://example.com:8080",
    };

    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, NULL);

    esp_websocket_client_start(client);

    while (esp_websocket_client_is_connected(client))
    {
        char *json_data = create_cjson();
        ESP_LOGI(TAG, "Sending JSON: %s", json_data);
        esp_websocket_client_send_text(client, json_data, strlen(json_data), portMAX_DELAY);
        free(json_data);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    esp_websocket_client_stop(client);
    esp_websocket_client_destroy(client);
    vTaskDelete(NULL);
}
