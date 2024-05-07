#include "freertos/FreeRTOS.h"
#include "freertos/task.h"          // Used for timer delay
#include "driver/i2c.h"
#include "i2c_util.h"               // header for this file

#define I2C_MASTER_SCL_IO 8       // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO 7        // GPIO number for I2C master data
#define I2C_MASTER_NUM I2C_NUM_0    // I2C port number
#define I2C_MASTER_FREQ_HZ 100000   // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE 0 // I2C master does not need buffer
#define I2C_MASTER_RX_BUF_DISABLE 0 // I2C master does not need buffer

#define SENSOR_ADDR 0x37           // sensor's I2C address
#define SENSOR_READ_REG 0x05       // the register to read from
#define WRITE_BIT I2C_MASTER_WRITE // I2C master write
#define READ_BIT I2C_MASTER_READ   // I2C master read
#define ACK_CHECK_EN 0x1           // I2C master will check ack from slave
#define ACK_CHECK_DIS 0x0          // I2C master will not check ack from slave
#define ACK_VAL 0x0                // I2C ack value
#define NACK_VAL 0x1               // I2C nack value
#define GPIO_OUTPUT_IO_4 4 // Example GPIO pin number

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode,
                              I2C_MASTER_TX_BUF_DISABLE,
                              I2C_MASTER_RX_BUF_DISABLE, 0);
}

esp_err_t mcp9808_read_temperature(float *temperature)
{
    // Set IO4 high to power temp IC
    esp_rom_gpio_pad_select_gpio(GPIO_OUTPUT_IO_4);
    gpio_set_direction(GPIO_OUTPUT_IO_4, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_OUTPUT_IO_4, 1);

    uint8_t data[2];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SENSOR_READ_REG, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &data[0], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data[1], I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    // printf("Attempting I2C");
    //     printf("Read raw I2C temperature: 0x%02x 0x%02x\n", data[0], data[1]); // Debugging

    //     // Convert the data to a temperature
    //     int16_t temp = data[0] << 8 | data[1];

    //     // Mask off the flag bits
    //     temp = temp & 0x1FFF;
    //     if (data[0] & 0x10)
    //     {                   // Check if the temperature is negative
    //         temp |= 0xE000; // Set the sign bits if negative
    //     }
    //     *temperature = temp * 0.125;
    //     printf("I2C Temperature: %.2f°C\n", *temperature);
    //     // ESP_LOGI("MCP9808", "Temperature: %.2f°C", temperature);


    if (ret == ESP_OK)
    {
        printf("Attempting I2C");
        printf("Read raw I2C temperature: 0x%02x 0x%02x\n", data[0], data[1]); // Debugging

        // Convert the data to a temperature
        int16_t temp = data[0] << 8 | data[1];

        // Mask off the flag bits
        temp = temp & 0x1FFF;
        if (data[0] & 0x10)
        {                   // Check if the temperature is negative
            temp |= 0xE000; // Set the sign bits if negative
        }
        *temperature = temp * 0.125;
        printf("I2C Temperature: %.2f°C\n", *temperature);
        // ESP_LOGI("MCP9808", "Temperature: %.2f°C", temperature);
    }
    else
    {
        printf("Failed to read temperature\n");
        // ESP_LOGE("MCP9808", "Failed to read temperature");
    }

    return ret;
}