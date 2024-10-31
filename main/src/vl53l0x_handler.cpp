#include "vl53l0x_handler.h"

#include "esp_log.h"

#include "vl53l0x.h"
#include "i2c.h"

static const char *TAG = "vl53l0x: ";

int compare(const void *a, const void *b)
{
    return (*(uint16_t *)a - *(uint16_t *)b);
}



uint16_t sensor_read(vl53l0x_t *vl53l0x)
{
    uint16_t range_measure = vl53l0x->sensor->readRangeSingleMillimeters();
    if (vl53l0x->sensor->timeoutOccurred())
    {
        ESP_LOGI(TAG, "TIMEOUT\r\n");
    }
    return range_measure;
}

uint16_t median_filter(vl53l0x_t *vl53l0x, int num_samples)
{
    uint16_t samples[num_samples];
    for (int i = 0; i < num_samples; i++)
    {
        samples[i] = sensor_read(vl53l0x);
        // printf("%d\n", samples[i]);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    qsort(samples, num_samples, sizeof(uint16_t), compare);
    // printf("Data: %d\n", samples[num_samples / 2]);
    return samples[num_samples / 2];
}


void sensor_task(void *pvParameters)
{
    vl53l0x_t *vl53l0x = (vl53l0x_t *)pvParameters;
    vl53l0x->range = median_filter(vl53l0x, 64);
    ESP_LOGI(TAG, "Measured Distance: %d mm", vl53l0x->range);
    vTaskDelete(NULL);
}

void sensor_start(vl53l0x_t *vl53l0x)
{
    vl53l0x->i2c->init();
    vl53l0x->sensor->setTimeout(500);
    while (!vl53l0x->sensor->init())
    {
        ESP_LOGW(TAG, "Failed to detect and initialize sensor!\r\n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vl53l0x->sensor->setMeasurementTimingBudget(35000);
    vl53l0x->sensor->setSignalRateLimit(0.4);
#if defined LONG_RANGE
    // lower the return signal rate limit (default is 0.25 MCPS)
    vl53l0x->sensor.setSignalRateLimit(0.1);
    // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    vl53l0x->sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    vl53l0x->sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
    // reduce timing budget to 20 ms (default is about 33 ms)
    vl53l0x->sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
    // increase timing budget to 200 ms
    vl53l0x->sensor.setMeasurementTimingBudget(200000);
#endif
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}