#include <stdio.h>
#include <stdbool.h>

#include "nvs_handle.h"
#include "wifi_handler.h"
#include "http_server.h"
#include "gpio_handler.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    nvs_init();
    gpios_setup();
    wifi_init_sta();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    http_server_configure();
}
