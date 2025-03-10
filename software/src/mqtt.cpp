//
// Created by chris on 7/10/23.
//
#include "mqtt.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastConnect = 0;
unsigned long lastPing = 0;

#define MQTT_TOPIC_BUFFER_LENGTH 64
#define MQTT_MESSAGE_BUFFER_LENGTH 64

char mqttTopicBuffer[MQTT_TOPIC_BUFFER_LENGTH];
char mqttMessageBuffer[MQTT_MESSAGE_BUFFER_LENGTH];

#ifdef ENABLE_SLOT_1
float lastCurrentRoomTemperature = -999;
float lastDesiredRoomTemperature = -999;
#endif
#ifdef ENABLE_SLOT_2
float lastCurrentRoomTemperatureH2 = -999;
float lastDesiredRoomTemperatureH2 = -999;
#endif
#ifdef ENABLE_SLOT_3
float lastCurrentRoomTemperatureH3 = -999;
float lastDesiredRoomTemperatureH3 = -999;
#endif

void sendPing() {
    if (client.connected()) {
        strncpy(mqttTopicBuffer, "virtualvitotrol/online", MQTT_TOPIC_BUFFER_LENGTH);
        strncpy(mqttMessageBuffer, "true", MQTT_MESSAGE_BUFFER_LENGTH);
        client.publish(mqttTopicBuffer, mqttMessageBuffer, false);
        lastPing = millis();
    }
}

#ifdef ENABLE_SLOT_1
void sendCurrentRoomTemperature() {
    if (client.connected()) {
        float f = currentRoomTemperature->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastCurrentRoomTemperature = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/currentRoomTemperature", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
void sendDesiredRoomTemperature() {
    if (client.connected()) {
        float f = desiredRoomTemperature->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastDesiredRoomTemperature = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/desiredRoomTemperature", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
#endif

#ifdef ENABLE_SLOT_2
void sendCurrentRoomTemperatureH2() {
    if (client.connected()) {
        float f = currentRoomTemperatureH2->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastCurrentRoomTemperatureH2 = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/currentRoomTemperatureH2", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
void sendDesiredRoomTemperatureH2() {
    if (client.connected()) {
        float f = desiredRoomTemperatureH2->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastDesiredRoomTemperatureH2 = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/desiredRoomTemperatureH2", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
#endif
#ifdef ENABLE_SLOT_3
void sendCurrentRoomTemperatureH3() {
    if (client.connected()) {
        float f = currentRoomTemperatureH3->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastCurrentRoomTemperatureH3 = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/currentRoomTemperatureH3", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
void sendDesiredRoomTemperatureH3() {
    if (client.connected()) {
        float f = desiredRoomTemperatureH3->getFloatValue();
        if (snprintf(mqttMessageBuffer, MQTT_TOPIC_BUFFER_LENGTH, "%.1f", f)) {
            lastDesiredRoomTemperatureH3 = f;
            strncpy(mqttTopicBuffer, "virtualvitotrol/desiredRoomTemperatureH3", MQTT_TOPIC_BUFFER_LENGTH);
            client.publish(mqttTopicBuffer, mqttMessageBuffer, true);
        }
    }
}
#endif


void sendConnected() {
    if (client.connected()) {
        bool connected = (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000);

        if (connected) {
            strncpy(mqttMessageBuffer, "true", MQTT_MESSAGE_BUFFER_LENGTH);
        } else {
            strncpy(mqttMessageBuffer, "false", MQTT_MESSAGE_BUFFER_LENGTH);
        }
        strncpy(mqttTopicBuffer, "virtualvitotrol/connected", MQTT_TOPIC_BUFFER_LENGTH);
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
                client.subscribe("virtualvitotrol/currentRoomTemperatureH2/set");
                client.subscribe("virtualvitotrol/desiredRoomTemperature/set");
                sendPing();
                sendConnected();
#ifdef ENABLE_SLOT_1
                sendCurrentRoomTemperature();
                sendDesiredRoomTemperature();
#endif
#ifdef ENABLE_SLOT_2
                sendCurrentRoomTemperatureH2();
                sendDesiredRoomTemperatureH2();
#endif
#ifdef ENABLE_SLOT_3
                sendCurrentRoomTemperatureH3();
                sendDesiredRoomTemperatureH3();
#endif
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
#ifdef ENABLE_SLOT_1
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
    } else
#endif
#ifdef ENABLE_SLOT_2
    if (strcmp(topic, "virtualvitotrol/currentRoomTemperatureH2/set") == 0) {
        float temp = strtof(mqttMessageBuffer, nullptr);
        currentRoomTemperatureH2->setValueReceivedByWifi(temp);
        sendCurrentRoomTemperatureH2();
        notifyCommandReceived();
    } else if (strcmp(topic, "virtualvitotrol/desiredRoomTemperatureH2/set") == 0) {
            float temp = strtof(mqttMessageBuffer, nullptr);
            desiredRoomTemperatureH2->setValueReceivedByWifi(temp);
            sendDesiredRoomTemperatureH2();
            notifyCommandReceived();
        } else
#endif
#ifdef ENABLE_SLOT_3
    if (strcmp(topic, "virtualvitotrol/currentRoomTemperatureH3/set") == 0) {
        float temp = strtof(mqttMessageBuffer, nullptr);
        currentRoomTemperatureH3->setValueReceivedByWifi(temp);
        sendCurrentRoomTemperatureH3();
        notifyCommandReceived();
    } else if (strcmp(topic, "virtualvitotrol/desiredRoomTemperatureH3/set") == 0) {
            float temp = strtof(mqttMessageBuffer, nullptr);
            desiredRoomTemperatureH3->setValueReceivedByWifi(temp);
            sendDesiredRoomTemperatureH3();
            notifyCommandReceived();
    } else
#endif
    {
        // not handled
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

#ifdef ENABLE_SLOT_1
        if (lastCurrentRoomTemperature != currentRoomTemperature->getFloatValue()) {
            sendCurrentRoomTemperature();
        }
        if (lastDesiredRoomTemperature != desiredRoomTemperature->getFloatValue()) {
            sendDesiredRoomTemperature();
        }
#endif
#ifdef ENABLE_SLOT_2
        if (lastCurrentRoomTemperatureH2 != currentRoomTemperatureH2->getFloatValue()) {
            sendCurrentRoomTemperatureH2();
        }
        if (lastDesiredRoomTemperatureH2 != desiredRoomTemperatureH2->getFloatValue()) {
            sendDesiredRoomTemperatureH2();
        }
#endif
#ifdef ENABLE_SLOT_3
        if (lastCurrentRoomTemperatureH3 != currentRoomTemperatureH3->getFloatValue()) {
            sendCurrentRoomTemperatureH3();
        }
        if (lastDesiredRoomTemperatureH3 != desiredRoomTemperatureH3->getFloatValue()) {
            sendDesiredRoomTemperatureH3();
        }
#endif
    }
}