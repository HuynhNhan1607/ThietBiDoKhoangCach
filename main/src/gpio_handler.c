#include "gpio_handler.h"

#include "driver/gpio.h"

#include "esp_mac.h"
/**
 * FreeRTOS
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_BUILDING GPIO_NUM_27

/**
 * Configure the ESP32 gpios (LED & button );
 */
void led_on()
{
    gpio_set_level(LED_BUILDING, 1);
}
void led_off()
{
    gpio_set_level(LED_BUILDING, 0);
}

void gpios_setup()
{
    /**
     * Configure the GPIO LED BUILDING
     */
    gpio_reset_pin(LED_BUILDING);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(LED_BUILDING, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_BUILDING, 0);

    // xTaskCreate(blink_led_task, "blink_led_task", 1024 * 2, NULL, 10, NULL);
}
