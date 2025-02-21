//
// Created by chris on 7/10/23.
//

#ifndef SOFTWARE_MQTT_H
#define SOFTWARE_MQTT_H

#include "configuration.h"

#ifndef MQTT_HOST
#include "wifi_mgr_portal.h"
#endif


#include <WiFiClient.h>
#include <PubSubClient.h>

#include "main.h"

void mqttSetup();
void mqttLoop();

#endif //SOFTWARE_MQTT_H
