#include <stdio.h>
#include <stdbool.h>
extern "C"
{
#include "nvs_handle.h"
#include "wifi_handler.h"

#include "gpio_handler.h"
#include "WebSocket.h"
}
#include "http_server.h"
#include "vl53l0x_handler.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


extern "C" {
void app_main(void);
}

VL53L0X vl53l0x_c;
vl53l0x_t vl53l0x = {
    .i2c = &i2c,
    .sensor = &vl53l0x_c,
    .range = 0};

void app_main(void)
{
    nvs_init();
    gpios_setup();
    wifi_init_sta();
    sensor_start(&vl53l0x);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    http_server_configure();
    // xTaskCreate(&websocket_client_task, "websocket_client_task", 8192, NULL, 5, NULL);
}
