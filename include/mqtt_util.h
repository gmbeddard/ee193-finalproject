#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H

typedef enum {
    MQTT_INIT,
    MQTT_PUBLISH,
    MQTT_END
} mqtt_cmd;

void mqtt_init();
void mqtt_publish(const char* topic, const char* message, mqtt_cmd cmd);

#endif