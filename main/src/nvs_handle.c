#include <stdio.h>

#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#include "nvs_handle.h"

static const char *TAG = "NVS_STORAGE";

void nvs_init()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}
void clear_nvs()
{
    int32_t arr[5] = {0};
    store_array(arr);
}
void store_array(int32_t *array)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        char key[16];
        sprintf(key, "LanDo_%d", i);
        err = nvs_set_i32(my_handle, key, array[i]);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) saving array[%d]!", esp_err_to_name(err), i);
        }
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) committing!", esp_err_to_name(err));
    }

    nvs_close(my_handle);
}

void load_array(int32_t *array)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        char key[16];
        sprintf(key, "LanDo_%d", i);
        err = nvs_get_i32(my_handle, key, &array[i]);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error (%s) reading array[%d]!", esp_err_to_name(err), i);
        }
    }
    nvs_close(my_handle);
}

void update_array_element(int index, int32_t value)
{
    if (index < 0 || index >= ARRAY_SIZE)
    {
        ESP_LOGE(TAG, "Index out of bounds");
        return;
    }

    int32_t array[ARRAY_SIZE];
    load_array(array);
    array[index] = value;
    store_array(array);
}

void add_new_element(int32_t value)
{
    int32_t array[ARRAY_SIZE];
    load_array(array);
    for (int i = ARRAY_SIZE - 1; i > 0; i--)
    {
        array[i] = array[i - 1];
    }
    array[0] = value;
    store_array(array);
}
