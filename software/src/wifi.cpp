#include "wifi.h"

#ifdef ESP8266
ESP8266WebServer server(80);
#endif
#ifdef ESP32
WebServer server(80);
#endif

void wifiNotifyCommandReceived() {
    lastCommandReceivedAt = millis();
}

//
//    THE HANDLERS
//    -> callbacks for URLs
//
void wifiHandleGetData() {
    String uri = server.uri();
    String datapointName = uri.substring(4);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        // TODO: iterate over read-only-datapoints
        server.send(404, "text/plain", "NOT FOUND");
    }

    server.send(200, "text/plain", String(datapoint->getFloatValue()));
}

void wifiHandleSetData() {
    String uri = server.uri();
    String datapointName = uri.substring(4);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        server.send(404, "text/plain", "NOT FOUND");
    }

    String strValue;
    float floatValue;
    bool hasValue = false;
    bool forceWrite = false;
    for (int i = 0; i < server.args(); i++) {
        if (server.argName(i) == "val") {
            strValue = server.arg(i).c_str();
            floatValue = strValue.toFloat();
            hasValue = true;
            break;
        }
        if (server.argName(i) == "force" || server.argName(i) == "forceWrite") {
            forceWrite = true;
        }
    }
    if (hasValue) {
        datapoint->setValueReceivedByWifi(floatValue, forceWrite);
        String ret = String(datapoint->getFloatValue());
        server.send(200, "text/plain", ret);
        wifiNotifyCommandReceived();
    } else {
        server.send(400, "text/plain", "val parameter is missing");
    }
}

void wifiHandleGetSource() {
    String uri = server.uri();
    String datapointName = uri.substring(10);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        // TODO: iterate over read-only-datapoints
        server.send(404, "text/plain", "NOT FOUND");
    }

    switch (datapoint->getSource()) {
        case DataSource::WIFI:
            server.send(200, "text/plain", "WIFI");
            break;
        case DataSource::HEATER:
            server.send(200, "text/plain", "HEATER");
            break;
    }
    server.send(200, "text/plain", "UNKNOWN");
}

void wifiHandleGetOverview() {
    String ret = "";
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        ret += "Heater is connected\r\n";
    } else {
        ret += "Heater is not connected\r\n";
    }
    if (preventCommunication()) {
        ret += "Communication is prevented\r\n";
    } else {
        ret += "Communication is not prevented\r\n";
    }
    unsigned long m = millis();
    ret += "\r\nLast com: " + String(m - lastHeaterCommandReceivedAt) + "ms";
    ret += "\r\nLast read: " + String(m - lastReadAt) + "ms";
    ret += " (" + String(readByte, HEX) + ")";
    ret += "\r\nLast message: " + String(m - lastMessageAt) + "ms";
    ret += "\r\nLast message - last read: " + String(lastMessageAt - lastReadAt) + "ms";
    ret += "\r\nLast response time: " + String(lastResponseTime) + "ms";
    ret += "\r\nLast valid message: " + String(m - lastValidMessageAt) + "ms";
    ret += "\r\nLast valid message with response: " + String(m - lastMessageWithResponseAt) + "ms";
    ret += "\r\nLast valid message without response: " + String(m - lastMessageWithoutResponseAt) + "ms";
    ret += "\r\nLast response: " + String(m - lastResponseTime) + "ms";
    ret += "\r\nUptime: " + String(m/1000) + "s\r\n\r\n";

    ret += "Receive Buffer:\r\n";
    ret += "len: " + String(buffer[3]);
    ret += "\r\ncontent: \r\n";
    for (int i = 0; i < buffer[3] + 16; i++) ret += String(buffer[i], HEX) + "-";

    ret += "\r\n\r\nResponse Buffer:\r\n";
    ret += "len: " + String(responseBuffer[3]);
    ret += "\r\ncontent: \r\n";
    for (int i = 0; i < responseBuffer[3]; i++) ret += String(responseBuffer[i], HEX) + "-";

    ret += "\r\n\r\nData points:\r\n";

    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        ret += "\r\n\r\nName: " + String(writableDataPoint[p]->getName()) + "\r\n";
        ret += "  Value: " + String(writableDataPoint[p]->getValue()) + "\r\n";
        ret += "  Send Value: " + String(writableDataPoint[p]->getSendValue(), HEX) + "\r\n";
        ret += "  Prevent communication: " + String(writableDataPoint[p]->preventCommunication()) + "\r\n";
        ret += "  Has valid value: " + String(writableDataPoint[p]->getHasValidValue()) + "\r\n";
        ret += "  Write pending: " + String(writableDataPoint[p]->wantsToSendValue()) + "\r\n";
        ret += "  Last send: " + String(m - writableDataPoint[p]->getLastSend()) + "ms ago\r\n";
        ret += "  Last set: " + String(m - writableDataPoint[p]->getLastSet()) + "ms ago\r\n";
        ret += "  Periodic send every " + String(writableDataPoint[p]->getPeriodicSend()) + "ms\r\n";
        ret += "  Prevent communication after " + String(writableDataPoint[p]->getPreventCommunicationAfter()) +
               "ms\r\n";
        p++;
    }

    server.send(200, "text/plain", ret);
}

void wifiHandleReadDatasets() {
    String ret = "{";
        for(int i = 0; i < DATASETS; i++) {
            Dataset *d = getDataset(i);
            if (d != nullptr) {
                ret += String(i) + ": [";
                for (int i = 0; i < d->len; i++) {
                    ret += String(d->data[i]);
                    if (i+1 < d->len) ret += ",";
                }
                ret += "{}]";
            }
        }
    ret += "}";
    server.send(200, "application/json", ret);
}

void wifiHandleReadRegisters() {
    String ret = "[";
    for (int i = 0; i <= 255; i++) {
        ret += String((uint8_t) getRegisterValue(i));
        if (i < 255) ret += ",";
    }
    ret += "]";
    server.send(200, "application/json", ret);
}

void wifiHandleIsPreventCommunication() {
    if (preventCommunication()) {
        server.send(200, "text/plain", "true");
    } else {
        server.send(200, "text/plain", "false");
    }
}

void wifiHandleIsConnected() {
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        server.send(200, "text/plain", "true");
    } else {
        server.send(200, "text/plain", "false");
    }
}

void wifiHandleSetSendDelay() {
    if (server.hasArg("delay")) sendDelay = server.arg("delay").toInt();
    server.send(200, "text/plain", String(sendDelay));
}

void wifiHandleReboot() {
    server.send(200, "text/plain", "ok");
    delay(100);
    ESP.restart();
}

// ... setup wifi
void setupWifi() {
    WiFi.begin(wifiSSID, wifiPassword);
    //WiFi.setAutoReconnect(true);
#ifdef ESP32
    WiFi.setHostname(wifiHost);
#endif

    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        server.on("/get" + String(writableDataPoint[p]->getName()), wifiHandleGetData);
        server.on("/set" + String(writableDataPoint[p]->getName()), HTTP_POST, wifiHandleSetData);
        server.on("/getSource" + String(writableDataPoint[p]->getName()), wifiHandleGetSource);
        debugPrint("dynamically created handler /get- and /set");
        debugPrintln(String(writableDataPoint[p]->getName()));
        p++;
    }

    server.on("/", wifiHandleGetOverview);
    server.on("/get", wifiHandleGetData);
    server.on("/set", HTTP_POST, wifiHandleSetData);
    server.on("/isConnected", wifiHandleIsConnected);
    server.on("/reboot", wifiHandleReboot);
    server.on("/registers", wifiHandleReadRegisters);
    server.on("/datasets", wifiHandleReadDatasets);
    server.on("/setSendDelay", wifiHandleSetSendDelay);

    debugPrintln("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    if (!MDNS.begin(wifiHost)) {
        debugPrintln("Error setting up MDNS responder!");
    }
    debugPrintln(WiFi.localIP());

    ElegantOTA.begin(&server);

    server.begin();
}

void wifiLoop() {
    server.handleClient();
}