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
        ESP_LOGI(TAG, "WebSocket connected - Event");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WebSocket disconnected - Event");
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
    if (root == NULL)
    {
        ESP_LOGE(TAG, "Failed to create cJSON object");
        return NULL;
    }
    cJSON_AddStringToObject(root, "id", "ESP32");
    cJSON_AddNumberToObject(root, "distance", data);

    char *json_string = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (json_string == NULL)
    {
        ESP_LOGE(TAG, "Failed to print JSON string");
    }
    return json_string;
}

void websocket_client_task(void *pvParameters)
{
    // while (1) // Vòng lặp để giữ task chạy liên tục
    // {
    const esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://192.168.2.179:8080/ws",
        .transport = WEBSOCKET_TRANSPORT_OVER_TCP,
    };

    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize WebSocket client");
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Chờ trước khi thử lại
    }

    esp_err_t err = esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, NULL);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to register WebSocket events: %s", esp_err_to_name(err));
        esp_websocket_client_destroy(client);
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Chờ trước khi thử lại
    }

    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    err = esp_websocket_client_start(client);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start WebSocket client: %s", esp_err_to_name(err));
        esp_websocket_client_destroy(client);
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Chờ trước khi thử lại
    }
    ESP_LOGE(TAG, "Start OKE");
    // Kiểm tra kết nối
    if (!esp_websocket_client_is_connected(client))
    {
        ESP_LOGE(TAG, "WebSocket not connected");
        // esp_websocket_client_stop(client);
        // esp_websocket_client_destroy(client);
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Chờ trước khi thử lại
    }

    // Gửi dữ liệu khi WebSocket đang kết nối
    while (esp_websocket_client_is_connected(client))
    {
        char *json_data = create_cjson();
        if (json_data == NULL)
        {
            ESP_LOGE(TAG, "Failed to create JSON data");
            break; // Thoát vòng lặp nếu không thể tạo JSON
        }

        ESP_LOGI(TAG, "Sending JSON: %s", json_data);
        esp_websocket_client_send_text(client, json_data, strlen(json_data), portMAX_DELAY);
        free(json_data);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGE(TAG, "Not Oke");

    // Dọn dẹp và giải phóng tài nguyên trước khi thử lại
    esp_websocket_client_stop(client);
    esp_websocket_client_destroy(client);

    ESP_LOGI(TAG, "Reconnecting WebSocket in 5 seconds...");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Chờ trước khi kết nối lại
    // }
    vTaskDelete(NULL);
}
