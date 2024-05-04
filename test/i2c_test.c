#include "i2c_util.h"

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI("MCP9808", "I2C initialized successfully");

    float temperature;
    while (1)
    {
        esp_err_t result = mcp9808_read_temperature(&temperature);
        vTaskDelay(10000 / portTICK_PERIOD_MS); // 10 second delay
    }
}
