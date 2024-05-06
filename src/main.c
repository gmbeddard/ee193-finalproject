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

// for time topic
char ascii_epoch_time[20]; // to store the epoch time
bool time_received;
RTC_DATA_ATTR double battery_val = 100.0;


void app_main()
{
    // Initialize the I2C bus and WiFi, MQTT
    strcpy(ascii_epoch_time, "1700000000"); // default place holder
    ESP_ERROR_CHECK(i2c_master_init());
    mqtt_init();


    while (1)
    {  
        time_received=false;
        mqtt_app_start(); // starting time subscriber client

        // Make full packet
        char message[50];

        // Calculate temperature from IC temperature sensor
        float mcp9808_temp;// = 14.5;
        mcp9808_read_temperature(&mcp9808_temp);
        char ic_temp[20];
        sprintf(ic_temp, "%f", mcp9808_temp);

        // Wait for epoch time retrieve -- by the MQTT event handler callback
        while (!time_received)
        {
            // printf("loop -- no time yet");
        }
        
        // Calculate battery
        // Seed the random number generator
        srand(time(NULL));
        int random_int = rand();
        double consumptn = (double)random_int / RAND_MAX * 0.5 - 0.5;  // [-0.5, 0]
        char batt[10];
        printf("Doing decrement: %lf - %lf = %lf\n", battery_val, consumptn, battery_val+consumptn);
        battery_val += consumptn;
        sprintf(batt, "%0.2f %%", battery_val);

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
        // mqtt_publish("czhao07/hw5/new_test", "test_publish", MQTT_END); // for MQTT test
        printf("\n\t=====\n");


        int minutes = 1;
        vTaskDelay(minutes * 1000 / portTICK_PERIOD_MS); // Delay for some minutes

        //esp_sleep_enable_timer_wakeup(3600 * 1000000); // 1 hour in microseconds
        esp_sleep_enable_timer_wakeup(60 * 1000000); // 1 hour in microseconds
        printf("Going into deep sleep for 1 hour...\n");
        esp_deep_sleep_start();
    }
}