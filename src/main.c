// Minimal code to initialize and read a value from the ADC
// Based on Espressif example:
// https://github.com/espressif/esp-idf/blob/f68c131e5603feca21659e92ad85f0c3369692fe/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c
// Steven Bell <sbell@ece.tufts.edu>
#include "FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"          // Used for timer delay
#include <math.h>               // for log and pow
#include "esp_adc/adc_oneshot.h"
#include "i2c_util.h"
#include "mqtt_util.h"
#include "esp_log.h"


/* Pick your ADC channel here.  Channels 0-4 correspond to GPIO 0-4, and are
 * on ADC 1. ADC 2 doesn't work on ESP32-C3 due to a silicon bug, so GPIO 5
 * isn't available. */
#define ADC_CHANNEL ADC_CHANNEL_0

// Steinhart-Hart Coefficients for the NTC thermistor
// Replace these with the correct values for your specific thermistor
const float A = 0.8026065509e-03;
const float B = 2.125420791e-04;
const float C = 0.6843260580e-07;

const float R1 = 10000;     // 10k Ohm resistor, voltage divider for thermistor
const float TH1_R0 = 100000;


void app_main()
{
    // Configure the ADC
    adc_oneshot_unit_init_cfg_t adc1_init_cfg = {
        .unit_id = ADC_UNIT_1};
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_new_unit(&adc1_init_cfg, &adc1_handle);

    // Configure the channel within the ADC
    adc_oneshot_chan_cfg_t adc1_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Default is 12 bits (max)
        .atten = ADC_ATTEN_DB_11};

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &adc1_cfg);
    // Skip calibration setup for now

    // Initialize the I2C bus and WiFi, MQTT
    ESP_ERROR_CHECK(i2c_master_init());
    mqtt_init();

    while (1)
    {
        // Sample the ADC and save the result
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);

        // Measure temperature from the thermistor voltage (Steinhart-Hart equation)
        // NOTE: here voltage over the divider resistor
        float voltage = adc_raw * 3.3 / 4095;
        float thermistorResistance = (3.3 * R1 / voltage) - R1;
        // printf("Thermistor resistance: %f\n", thermistorResistance); // Debugging

        // Calculate temperature using the Steinhart-Hart equation
        float steinhart;
        steinhart = 1.0 / (A + B * log(thermistorResistance) + C * pow(log(thermistorResistance), 3)); // Convert to Kelvin
        steinhart -= 273.15;
        char therm_temp[10];
        sprintf(therm_temp, "%f", steinhart);
        printf("Thermistor temperature: %s\n", therm_temp);

        // Calculate temperature from IC temperature sensor
        float mcp9808_temp;
        mcp9808_read_temperature(&mcp9808_temp);
        char ic_temp[10];
        sprintf(ic_temp, "%f", mcp9808_temp);

        // Print and publish the temperature, disconnect and clean up
        mqtt_publish("czhao07/hw5/thermistor_temp", therm_temp, MQTT_INIT);
        // mqtt_publish("czhao07/hw5/test", "test_publish", MQTT_PUBLISH);
        mqtt_publish("czhao07/hw5/ic_temp", ic_temp, MQTT_END);
        printf("\t=====\n");


        int minutes = 10;
        vTaskDelay(minutes * 60000 / portTICK_PERIOD_MS); // Delay for some minutes
    }

    // If we had other things to do with the ADC, we could release it with
    // adc_oneshot_del_unit(adc1_handle);
}