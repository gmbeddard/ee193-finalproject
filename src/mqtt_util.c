// Minimal example of using ESP-MQTT to publish messages to a broker
// Complete documentation is here:
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/protocols/mqtt.html
//
// See also the example code, which demonstrates how to receive messages:
// https://github.com/espressif/esp-idf/blob/5f4249357372f209fdd57288265741aaba21a2b1/examples/protocols/mqtt/tcp/main/app_main.c

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" // Used for timer delay
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_client.h"
#include "minimal_wifi.h"
#include "mqtt_util.h"      // header for this file
#include "esp_log.h"


#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

#define BROKER_URI "mqtt://en1-pi.eecs.tufts.edu"
#define SUBSCRIBE_TOPIC "time"


// Initialize the MQTT client
esp_mqtt_client_handle_t client;

void process_received_message(const char *data, int data_len)
{
    // Process the received message containing time information
    // Example: Parse the message and extract time data
    // Time data could be in a specific format, such as Unix Epoch time or a formatted timestamp
    // You can use sscanf, strtok, or any parsing method suitable for your message format
    // Example parsing:
    long int received_time;
    sscanf(data, "%ld", &received_time); // Assuming the message contains a long integer representing time
    ESP_LOGI("MQTT", "Received time: %ld", received_time);
}

static esp_event_handler_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MQTT", "Connected to broker");
            esp_mqtt_client_subscribe(event->client, SUBSCRIBE_TOPIC, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("MQTT", "Disconnected from broker");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI("MQTT", "Received message: %.*s", event->data_len, event->data);
            // Process the received message containing time information
            // process_received_message(event->data, event->data_len);
            break;
        default:
            break;
    }
    return ESP_OK;
}


void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = BROKER_URI,
        };

    esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, mqtt_client);
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));
    printf("starting the subscriber");
}

void mqtt_init()
/* initialization for the whole MQTT system -- nvs, wifi, etc. */
{
    // Enable Flash (aka non-volatile storage, NVS)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Normally we'd need to initialize the event loop and the network stack,
    // but both of these will be done when we connect to WiFi.
    printf("Connecting to WiFi %s ...", WIFI_SSID);
    wifi_connect(WIFI_SSID, WIFI_PASS);
    printf("Connected to WiFi %s!\n", WIFI_SSID);
}


void mqtt_publish(const char* topic, const char* message, mqtt_cmd cmd)
{
    // Initialize the MQTT client
    // Read the documentation for more information on what you can configure:
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/protocols/mqtt.html
    if (cmd == MQTT_INIT)
    {
        esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = BROKER_URI,
        };
        client = esp_mqtt_client_init(&mqtt_cfg);
        ESP_ERROR_CHECK(esp_mqtt_client_start(client));
        // printf("Sending MQTT message in 5 seconds...\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    // You'll want to call esp_mqtt_client_publish(client, ...) to send your message.
    // See https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/protocols/mqtt.html#_CPPv423esp_mqtt_client_publish24esp_mqtt_client_handle_tPKcPKciii
    esp_mqtt_client_publish(client, topic, message, 0, 0, 0);

    // clean up the client
    if (cmd == MQTT_END) {
        ESP_ERROR_CHECK(esp_mqtt_client_destroy(client));
    }
    printf("Message sent to topic %s!\n", topic);
}

