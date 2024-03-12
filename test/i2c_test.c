#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO    18    // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO    19    // GPIO number for I2C master data
#define I2C_MASTER_NUM       I2C_NUM_0 // I2C port number
#define I2C_MASTER_FREQ_HZ   100000    // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0  // I2C master does not need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0  // I2C master does not need buffer

#define SENSOR_ADDR                 0x18  // sensor's I2C address
#define SENSOR_READ_REG             0x03  // the register to read from
#define WRITE_BIT                   I2C_MASTER_WRITE // I2C master write
#define READ_BIT                    I2C_MASTER_READ  // I2C master read
#define ACK_CHECK_EN                0x1   // I2C master will check ack from slave
#define ACK_CHECK_DIS               0x0   // I2C master will not check ack from slave
#define ACK_VAL                     0x0   // I2C ack value
#define NACK_VAL                    0x1   // I2C nack value

static esp_err_t i2c_master_init(void) {
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

static esp_err_t sensor_read_byte(uint8_t reg_addr, uint8_t *data) {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg_addr, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


void app_main(void) {
    ESP_ERROR_CHECK(i2c_master_init());
    printf("I2C initialized successfully\n");

    uint8_t sensor_data;
    esp_err_t ret = sensor_read_byte(SENSOR_READ_REG, &sensor_data);
    if (ret == ESP_OK) {
        printf("Read from sensor: %d\n", sensor_data);
    } else {
        printf("Failed to read from sensor\n");
    }
}
