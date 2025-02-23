//
// Created by chris on 7/10/23.
//
#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastConnect = 0;
unsigned long lastPing = 0;

char mqttTopicBuffer[64];
char mqttMessageBuffer[32];

float lastCurrentRoomTemperature = -999;
float lastDesiredRoomTemperature = -999;

void sendPing() {
    if (client.connected()) {
        strcpy(mqttTopicBuffer, "virtualvitotrol/online");
        strcpy(mqttMessageBuffer, "true");
        client.publish(mqttTopicBuffer, mqttMessageBuffer, false);
        lastPing = millis();
    }
}

void sendCurrentRoomTemperature() {
    if (client.connected()) {
        float f = currentRoomTemperature->getFloatValue();
        if (snprintf(mqttMessageBuffer, 32, "%.1f", f)) {
            lastCurrentRoomTemperature = f;
            strcpy(mqttTopicBuffer, "virtualvitotrol/currentRoomTemperature");
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}

void sendDesiredRoomTemperature() {
    if (client.connected()) {
        float f = desiredRoomTemperature->getFloatValue();
        if (snprintf(mqttMessageBuffer, 32, "%.1f", f)) {
            lastDesiredRoomTemperature = f;
            strcpy(mqttTopicBuffer, "virtualvitotrol/desiredRoomTemperature");
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}

void sendConnected() {
    if (client.connected()) {
        bool connected = (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000);

        if (connected) {
            strcpy(mqttMessageBuffer, "true");
        } else {
            strcpy(mqttMessageBuffer, "false");
        }
        strcpy(mqttTopicBuffer, "virtualvitotrol/connected");
        client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
    }
}

void mqttReconnect() {
#ifndef MQTT_USER
    const char* MQTT_USER = wifiMgrGetConfig("MQTT_USER");
    if (MQTT_USER == nullptr) return;
#endif
#ifndef MQTT_PASS
    const char* MQTT_PASS = wifiMgrGetConfig("MQTT_PASS");
    if (MQTT_PASS == nullptr) return;
#endif
    if (!client.connected()) {
	    if (!WiFi.isConnected()) return;
        if (millis() - lastConnect > 5000) {
            debugPrintln("Reconnecting...");
            const char* hostname = WiFi.getHostname();
            if (!client.connect(hostname, MQTT_USER, MQTT_PASS, "virtualvitotrol/online", 1, true, "false")) {
                debugPrint("failed, rc=");
                debugPrint(client.state());
                debugPrintln(" retrying in 5 seconds");
            } else {
                debugPrint("success");
                client.subscribe("virtualvitotrol/currentRoomTemperature/set");
                client.subscribe("virtualvitotrol/desiredRoomTemperature/set");
                sendPing();
                sendConnected();
                sendCurrentRoomTemperature();
                sendDesiredRoomTemperature();
            }
            lastConnect = millis();
        }
    }
}

void mqttHandleMessage(char *topic, byte *payload, unsigned int length) {
    if (length > 31) return;
    unsigned int i;
    for (i = 0; i < length; i++) mqttMessageBuffer[i] = (char) payload[i];
    mqttMessageBuffer[i] = '\0';
    if (strcmp(topic, "virtualvitotrol/currentRoomTemperature/set") == 0) {
        float temp = strtof(mqttMessageBuffer, nullptr);
        currentRoomTemperature->setValueReceivedByWifi(temp);
        sendCurrentRoomTemperature();
        notifyCommandReceived();
    } else if (strcmp(topic, "virtualvitotrol/desiredRoomTemperature/set") == 0) {
        float temp = strtof(mqttMessageBuffer, nullptr);
        desiredRoomTemperature->setValueReceivedByWifi(temp);
        sendDesiredRoomTemperature();
        notifyCommandReceived();
    }
}

void mqttSetup() {
#ifndef MQTT_HOST
    const char* MQTT_HOST = wifiMgrGetConfig("MQTT_HOST");
    if (MQTT_HOST != nullptr) {
        client.setServer(MQTT_HOST, 1883);
        client.setCallback(mqttHandleMessage);
    }
    wifiMgrPortalAddConfigEntry("MQTT Host", "MQTT_HOST", PortalConfigEntryType::STRING, false, true);
    wifiMgrPortalAddConfigEntry("MQTT Username", "MQTT_USER", PortalConfigEntryType::STRING, false, true);
    wifiMgrPortalAddConfigEntry("MQTT Password", "MQTT_PASS", PortalConfigEntryType::STRING, false, true);
#else
    client.setServer(MQTT_HOST, 1883);
#endif
    client.setCallback(mqttHandleMessage);
    debugPrint("Configured MQTT_HOST ");
    debugPrintln(MQTT_HOST);
}

void mqttLoop() {
    if (!client.connected()) {
        mqttReconnect();
    }
    if (client.connected()) {
        client.loop();

        if ((millis() - lastPing) > 30000) {
            sendPing();
            sendConnected();
        }

        if (lastCurrentRoomTemperature != currentRoomTemperature->getFloatValue()) {
            sendCurrentRoomTemperature();
        }
        if (lastDesiredRoomTemperature != desiredRoomTemperature->getFloatValue()) {
            sendDesiredRoomTemperature();
        }
    }
}