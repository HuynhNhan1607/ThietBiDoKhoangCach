#pragma once
#include "esp_err.h"

#define HTTP_SERVER_TASK_CORE_ID 0
#define HTTP_SERVER_TASK_STACK_SIZE 4096
#define HTTP_SERVER_TASK_PRIORITY 4

esp_err_t http_server_configure();
