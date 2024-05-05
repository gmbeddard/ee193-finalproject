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
#include <time.h>
#include <string.h>
#include "lwip/apps/sntp.h"
#include "esp_sleep.h"

#define MQTT_SUBSCRIBE_TOPIC = "time";


/* Pick your ADC channel here.  Channels 0-4 correspond to GPIO 0-4, and are
 * on ADC 1. ADC 2 doesn't work on ESP32-C3 due to a silicon bug, so GPIO 5
 * isn't available. */
//#define ADC_CHANNEL ADC_CHANNEL_0

// Steinhart-Hart Coefficients for the NTC thermistor
// Replace these with the correct values for your specific thermistor
const float A = 0.8026065509e-03;
const float B = 2.125420791e-04;
const float C = 0.6843260580e-07;

const float R1 = 10000;     // 10k Ohm resistor, voltage divider for thermistor
const float TH1_R0 = 100000;
char ascii_epoch_time[20] = "1700000000"; // to store the epoch time


void app_main()
{
    // Initialize the I2C bus and WiFi, MQTT
    ESP_ERROR_CHECK(i2c_master_init());
    mqtt_init();

    while (1)
    {  
        mqtt_app_start(); // starting time subscriber client

        // Make full packet
        char message[100];

        // Calculate temperature from IC temperature sensor
        float mcp9808_temp = 14.5;
        //mcp9808_read_temperature(&mcp9808_temp);
        char ic_temp[20];
        sprintf(ic_temp, "%f", mcp9808_temp);

        // Calculate epoch time
        // time_t current_time;
        // Assuming 20 characters is enough for the ASCII representation
        // sprintf(ascii_epoch_time, "%ld", (long)current_time); // Convert the Epoch time to ASCII string
        // int ret = sscanf(data, "%ld", &ascii_epoch_time); 
        strcpy(ascii_epoch_time, "1700000000");
        
        // fprintf(stdout, "Time: %u\n", (unsigned)time(NULL));
        printf(ascii_epoch_time);

        // Calculate battery
        int count = 100;
        char batt[10];
        sprintf(batt, "%d %%", count);
        count = count - 1;

        // Put it all together
        strcpy(message, ascii_epoch_time);
        strcat(message, ",");
        strcat(message, ic_temp);
        strcat(message, ",");
        strcat(message, batt);
        printf("\n\t=====\n");
        printf(message);

        // Print and publish the temperature, disconnect and clean up
        mqtt_publish("teamF/node1/tempupdate", message, MQTT_INIT);
        mqtt_publish("czhao07/hw5/new_test", "test_publish", MQTT_INIT);
        // mqtt_publish("czhao07/hw5/ic_temp", ic_temp, MQTT_END);
        printf("\n\t=====\n");


        int minutes = 1;
        vTaskDelay(minutes * 1000 / portTICK_PERIOD_MS); // Delay for some minutes

        //esp_sleep_enable_timer_wakeup(3600 * 1000000); // 1 hour in microseconds
        esp_sleep_enable_timer_wakeup(60 * 1000000); // 1 hour in microseconds
        printf("Going into deep sleep for 1 hour...\n");
        esp_deep_sleep_start();
    }
}