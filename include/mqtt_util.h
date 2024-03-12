#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H

void mqtt_init();
void mqtt_publish(const char* topic, const char* message);

#endif