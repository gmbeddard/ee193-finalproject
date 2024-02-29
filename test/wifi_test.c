#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "minimal_wifi.h"

// Configure the WiFi network settings here
#define WIFI_SSID      "Tufts_Wireless"
#define WIFI_PASS      ""

static const char *TAG = "wifi demo";

void app_main(void)
{
    // ESP_LOGI is a macro that prints stuff to stdout (the USB UART).  You can
    // define various tags and use multiple log levels, and then use macros or runtime
    // switches to turn them on or off.
    // See https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/system/log.html
    ESP_LOGI(TAG, "STARTING APPLICATION");

    // Initialize Flash non-volatile storage (NVS)
    // The default WiFi configuration stores information about current/past networks
    // in flash, which is useful for IoT applications where a customer has to configure
    // the device for their own network.  You can turn this off by setting the
    // `wifi_init_config_t` flag `nvs_enable` to false and removing this code.
    // See https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-guides/wifi.html?highlight=nvs#wi-fi-nvs-flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    // `ESP_ERROR_CHECK` is a macro which checks that the return value of a function is
    // `ESP_OK`.  If not, it prints some debug information and aborts the program.
    // If you get a boot loop, it's probably because ESP_ERROR_CHECK failed somewhere.
    ESP_ERROR_CHECK(ret);

    // The ESP32 can operate as either a WiFi client ("STA" mode) or as an
    // access point ("AP" mode).  We'll be using STA mode here.
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    printf("Connecting to %s\n", WIFI_SSID);
    wifi_connect(WIFI_SSID, WIFI_PASS);
    printf("Connected!\n");

    // Once we're up and connected, print some useful information
    while(1) {
        // MAC address
        uint8_t mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, mac);
        printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x    ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        // IP address - this is handled by the network stack (aka netif)
        esp_netif_ip_info_t ip_info;
        ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif_get_default_netif(), &ip_info));
        printf("IP: %ld.%ld.%ld.%ld    ", ip_info.ip.addr & 0xFF, ip_info.ip.addr >> 8 & 0xFF, ip_info.ip.addr >> 16 & 0xFF, ip_info.ip.addr >> 24 & 0xFF);

        // RSSI (signal strength)
        int rssi;
        esp_wifi_sta_get_rssi(&rssi);
        printf("RSSI: %d\n", rssi);

        vTaskDelay(100);
    }

}