// Minimal code to initialize and read a value from the ADC
// Based on Espressif example:
// https://github.com/espressif/esp-idf/blob/f68c131e5603feca21659e92ad85f0c3369692fe/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c
// Steven Bell <sbell@ece.tufts.edu>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "esp_adc/adc_oneshot.h"

/* Pick your ADC channel here.  Channels 0-4 correspond to GPIO 0-4, and are
 * on ADC 1. ADC 2 doesn't work on ESP32-C3 due to a silicon bug, so GPIO 5
 * isn't available. */
#define ADC_CHANNEL ADC_CHANNEL_0

void app_main() {
    // Configure the ADC
    adc_oneshot_unit_init_cfg_t adc1_init_cfg = {
        .unit_id = ADC_UNIT_1
    };
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_new_unit(&adc1_init_cfg, &adc1_handle);


    // Configure the channel within the ADC
    adc_oneshot_chan_cfg_t adc1_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Default is 12 bits (max)
        .atten = ADC_ATTEN_DB_11
    };

    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &adc1_cfg);

    // Skip calibration setup for now

    while(1){
        // Sample the ADC and save the result
        int adc_raw;
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw);

        // Skip calibration conversion

        printf("ADC raw: %d\n", adc_raw);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // If we had other things to do with the ADC, we could release it with
    // adc_oneshot_del_unit(adc1_handle);
    send_mqtt();
}