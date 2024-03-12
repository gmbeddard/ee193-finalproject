#ifndef I2C_UTIL_H
#define I2C_UTIL_H

esp_err_t i2c_master_init(void);
esp_err_t mcp9808_read_temperature(float *temperature);

#endif