// Minimal code to initialize and read a value from the ADC
// Based on Espressif example:
// https://github.com/espressif/esp-idf/blob/f68c131e5603feca21659e92ad85f0c3369692fe/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c
// Steven Bell <sbell@ece.tufts.edu>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "esp_adc/adc_oneshot.h"
#include <math.h>
#include "mqtt_util.h"

/* Pick your ADC channel here.  Channels 0-4 correspond to GPIO 0-4, and are
 * on ADC 1. ADC 2 doesn't work on ESP32-C3 due to a silicon bug, so GPIO 5
 * isn't available. */
#define ADC_CHANNEL ADC_CHANNEL_0

// Steinhart-Hart Coefficients for the NTC thermistor
// Replace these with the correct values for your specific thermistor
const float A = 0.8026065509e-03;
const float B = 2.125420791e-04;
const float C = 0.6843260580e-07;

const float seriesResistor = 10000;
const float R0 = 100000;


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

    // connect to WiFi and MQTT
    mqtt_init();

    while (1)
    {
        // Sample the ADC and save the result
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);
        // printf("ADC raw: %d\n", adc_raw);

        // Example of using a thermistor to measure temperature
        float voltage = adc_raw * 3.3 / 4095;
        float thermistorResistance = (3.3 * seriesResistor / voltage) - seriesResistor;
        printf("Thermistor resistance: %f\n", thermistorResistance);

        // Calculate temperature using the Steinhart-Hart equation
        float steinhart;
        steinhart = 1.0 / (A + B * log(thermistorResistance) + C * pow(log(thermistorResistance), 3)); // Convert to Kelvin
        steinhart -= 273.15;

        // Calculate temperature from IC temperature sensor
        char ic_temp[10] = "0";

        // Print and publish the temperature
        char therm_temp[10];
        sprintf(therm_temp, "%f", steinhart);
        mqtt_publish("czhao07/hw5/thermistor_temp", therm_temp);
        mqtt_publish("czhao07/hw5/ic_temp", ic_temp);
        printf("Therm Temperature: %s\n\t======\n", therm_temp);


        vTaskDelay(5 * 60000 / portTICK_PERIOD_MS); // Delay for 5 min
    }

    // If we had other things to do with the ADC, we could release it with
    // adc_oneshot_del_unit(adc1_handle);
}