//
// Created by chris on 7/10/23.
//

#ifndef SOFTWARE_MQTT_H
#define SOFTWARE_MQTT_H

#include "configuration.h"

#ifdef MQTT_HOST

#include <WiFiClient.h>
#include <PubSubClient.h>

#include "main.h"

#endif //ifdef MQTT_HOST

#define MQTT_TOPIC_BUFFER_SIZE 32
#define MQTT_VALUE_BUFFER_SIZE 16

void mqttSetup();
void mqttLoop();

#endif //SOFTWARE_MQTT_H
