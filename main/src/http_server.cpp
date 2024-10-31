#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdbool.h"

#include "esp_http_server.h"

#include "esp_err.h"
#include "esp_log.h"

#include "http_server.h"

#include "vl53l0x_handler.h"

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
    char recv_buf[1024];              // Bộ đệm để lưu chuỗi nhận
    int total_len = req->content_len; // Độ dài toàn bộ nội dung
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
    int number = atoi(recv_buf);

    // printf("Number is: %d\n", number);
    char resp_str[50];
    uint16_t data = median_filter(&vl53l0x, 64);
    snprintf(resp_str, sizeof(resp_str), "%d", data);
    httpd_resp_send(req, resp_str, strlen(resp_str));

    return ESP_OK;
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
    return ESP_OK;
}