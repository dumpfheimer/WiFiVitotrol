//
// Created by chris on 7/10/23.
//
#include "mqtt.h"

#ifdef MQTT_HOST

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastConnect = 0;

void mqttReconnect() {
    if (!client.connected()) {
	    if (!WiFi.isConnected()) return;
        if (millis() - lastConnect > 5000) {
            debugPrintln("Reconnecting...");
            if (!client.connect("VirtualVitotrol", MQTT_USER, MQTT_PASS)) {
                debugPrint("failed, rc=");
                debugPrint(client.state());
                debugPrintln(" retrying in 5 seconds");
            } else {
                debugPrint("success");
            }
            lastConnect = millis();
        }
    }
}

void mqttHandleMessage(char *topic, byte *payload, unsigned int length) {
    char* startBuffer = (char*) payload;
    char* endBuffer = (char*) (payload + length);
    if (strcmp(topic, "virtualvitotrol/currentRoomTemperature/set") == 0) {
        float temp = strtof(startBuffer, &endBuffer);
        currentRoomTemperature->setValueReceivedByWifi(temp);
    } else if (strcmp(topic, "virtualvitotrol/desiredRoomTemperature/set") == 0) {
        float temp = strtof(startBuffer, &endBuffer);
        desiredRoomTemperature->setValueReceivedByWifi(temp);
    }
}

void mqttSetup() {
    client.setServer(MQTT_HOST, 1883);
    client.setCallback(mqttHandleMessage);
    client.setBufferSize(350);
}

void mqttLoop() {
    if (!client.connected()) {
        mqttReconnect();
    }
    if (client.connected()) {
        client.loop();

        MqttDatapoint *nextDatapoint = getNextDataPoint();
        nextDatapoint->loop();
    }
}

#else
void mqttSetup() {}
void mqttLoop() {}
#endif