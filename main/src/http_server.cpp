#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdbool.h"

#include "esp_http_server.h"

#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"
#include "mdns.h"
#include "http_server.h"
#include "vl53l0x_handler.h"
extern "C"
{
#include "nvs_handle.h"
}

extern vl53l0x_t vl53l0x;

static const char *TAG = "HTTP_HANDLE";

extern const unsigned char style_start[] asm("_binary_style_css_start");
extern const unsigned char style_end[] asm("_binary_style_css_end");

extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

extern const unsigned char script_start[] asm("_binary_script_js_start");
extern const unsigned char script_end[] asm("_binary_script_js_end");

static httpd_handle_t http_server_handle = NULL;

extern uint16_t data;

static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "index.html requested");

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_start, index_end - index_start);

    return ESP_OK;
}

static esp_err_t http_server_style_css_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "style.css requested");

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)style_start, style_end - style_start);

    return ESP_OK;
}

static esp_err_t http_server_script_js_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "script.js requested");

    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)script_start, script_end - script_start);

    return ESP_OK;
}

static esp_err_t http_receive_handle(httpd_req_t *req)
{
    char recv_buf[1024];              
    int total_len = req->content_len; 
    int cur_len = 0;
    int received = 0;
    if (total_len >= 1024)
    {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    while (cur_len < total_len)
    {
        received = httpd_req_recv(req, recv_buf + cur_len, total_len - cur_len);
        if (received <= 0)
        {
            if (received == HTTPD_SOCK_ERR_TIMEOUT)
            {
                continue;
            }
            return ESP_FAIL;
        }
        cur_len += received;
    }
    recv_buf[cur_len] = '\0';
    if (strcmp(recv_buf, "Clear") == 0)
    {
        clear_nvs();
    }
    else
    {
        // printf("Number is: %d\n", number);
    // char resp_str[50];
    
    // snprintf(resp_str, sizeof(resp_str), "%d", data);
    // httpd_resp_send(req, resp_str, strlen(resp_str));

    int DataToSend[5];
    if (strcmp(recv_buf, "Measuring") == 0)
    {
    uint16_t MeasuringData = median_filter(&vl53l0x, 16);
    add_new_element((int32_t)MeasuringData);
    }
    load_array((int32_t*)DataToSend);
    // Tạo dữ liệu JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *array = cJSON_CreateIntArray(DataToSend, 5);
    cJSON_AddItemToObject(root, "data", array);
    char *json_response = cJSON_Print(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_response, strlen(json_response));

    // Giải phóng bộ nhớ
    cJSON_Delete(root);
    free(json_response);
    }

    return ESP_OK;
}

void start_mdns_service()
{
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("distance-measurer"));
    ESP_LOGI(TAG, "mDNS hostname set to: distance-measurer");

    ESP_ERROR_CHECK(mdns_instance_name_set("Distance Measuring Device"));
    ESP_ERROR_CHECK(mdns_service_add("Web Server", "_http", "_tcp", 80, NULL, 0));

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"project", "Distance Measurement"},
    };
    ESP_ERROR_CHECK(mdns_service_txt_set("_http", "_tcp", serviceTxtData, 2));
}



esp_err_t http_server_configure(void)
{
    // Generate the default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // The core that the HTTP server will run on
    config.core_id = HTTP_SERVER_TASK_CORE_ID;

    // Adjust the default priority to 1 less than the wifi application task
    config.task_priority = HTTP_SERVER_TASK_PRIORITY;

    // Bump up the stack size (default is 4096)
    config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

    // Increase uri handlers
    config.max_uri_handlers = 20;

    // Increase the timeout limits
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    ESP_LOGI(TAG,
             "http_server_configure: Starting server on port: '%d' with task priority: '%d'",
             config.server_port,
             config.task_priority);

    // Start the httpd server
    if (httpd_start(&http_server_handle, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

        // register index.html handler
        httpd_uri_t index_html = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = http_server_index_html_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &index_html);

        // register app.css handler
        httpd_uri_t style_css = {
            .uri = "/style.css",
            .method = HTTP_GET,
            .handler = http_server_style_css_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &style_css);

        // register app.js handler
        httpd_uri_t script_js = {
            .uri = "/script.js",
            .method = HTTP_GET,
            .handler = http_server_script_js_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &script_js);

        httpd_uri_t receive_uri = {
            .uri = "/receive",
            .method = HTTP_POST,            // Phương thức HTTP
            .handler = http_receive_handle, // Hàm xử lý yêu cầu POST
            .user_ctx = NULL};
        httpd_register_uri_handler(http_server_handle, &receive_uri);
    }
    start_mdns_service();
    return ESP_OK;
}