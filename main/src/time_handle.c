#include <stdio.h>
#include <time.h>
#include "esp_sntp.h"
#include "time_handle.h"
#include "gpio_handler.h"

void time_sync_notification_cb(struct timeval *tv)
{
    printf("Time synchronization event occurred\n");
}

void init_time_sync()
{
    printf("Initializing SNTP\n");

    // Cấu hình múi giờ Việt Nam
    setenv("TZ", "ICT-7", 1); // ICT là múi giờ UTC+7
    tzset();

    // Thiết lập SNTP
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    // Chờ cho đến khi thời gian được đồng bộ hóa
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2024 - 1900) && ++retry < retry_count)
    {
        printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Chờ 1 giây trước khi thử lại
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (retry == retry_count)
    {
        printf("Failed to synchronize time with NTP server\n");
        esp_restart();
    }
    else
    {
        led_on();
        printf("Time synchronized successfully\n");
    }
}
