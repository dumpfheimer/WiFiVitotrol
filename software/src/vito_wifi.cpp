#include "vito_wifi.h"

XWebServer server(80);

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
        return;
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
        return;
    }

    String strValue;
    float floatValue;
    bool hasValue = false;
    bool forceWrite = false;
    for (int i = 0; i < server.args(); i++) {
        if (server.argName(i) == "val") {
            strValue = server.arg(i);
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
        notifyCommandReceived();
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
        return;
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

void wifiHandleGetLinkState() {
    server.send(200, "text/plain", linkState);
}

void wifiHandleGetOverview() {
    String ret = "";
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        ret += "Heater is connected\n";
    } else {
        ret += "Heater is not connected\n";
    }
    for (uint16_t s = 0; s < getSlotCount(); s++) {
        uint8_t slot = getSlot(s);
        ret += "Slot " + String(slot) + ": ";
        if (preventCommunication(slot)) {
            ret += "Communication is prevented\n";
        } else {
            ret += "Communication is not prevented\n";
        }
    }
#ifdef FIND_DEVICE_ID
    if (deviceIdFound) ret += "DeviceID found: 0x" + String(getRegisterValue(0xF9), HEX) + "\n";
    else ret += "Scanning for DeviceID. Currently at 0x" + String(getRegisterValue(0xF9), HEX) + "\n";
#endif
    ret += "Link state: " + String(linkState);
    ret += "\nBuild date: ";
    ret += __DATE__;
    ret += " ";
    ret += __TIME__;
    ret += "\n";
    unsigned long m = millis();
    ret += "\nLast com: " + String(m - lastHeaterCommandReceivedAt) + "ms";
    ret += "\nLast read: " + String(m - lastReadAt) + "ms";
    ret += " (" + String(readByte, HEX) + ")";
    ret += "\nLast message: " + String(m - lastMessageAt) + "ms";
    ret += "\nLast message - last read: " + String(lastMessageAt - lastReadAt) + "ms";
    ret += "\nLast valid message: " + String(m - lastValidMessageAt) + "ms";
    ret += "\nLast valid broadcast message: " + String(m - lastBroadcastMessage) + "ms";
    ret += "\nLast valid message with response: " + String(m - lastMessageWithResponseAt) + "ms";
    ret += "\nLast valid message without response: " + String(m - lastMessageWithoutResponseAt) + "ms";
    ret += "\nUptime: " + String(m/1000) + "s\n\n";

    ret += "Receive Buffer:\n";
    ret += "len: " + String(buffer[3]);
    ret += "\nptr: " + String((unsigned) &buffer, HEX);
    ret += "\ncontent: \n";
    for (int i = 0; i < buffer[3]; i++) ret += String(buffer[i], HEX) + "-";
    ret += "\n\nLast message:\n";
    ret += "len: " + String(lastValidMessage[3]);
    ret += "\nptr: " + String((unsigned) &lastValidMessage, HEX);
    ret += "\ncontent: \n";
    for (int i = 0; i < lastValidMessage[3]; i++) ret += String(lastValidMessage[i], HEX) + "-";
    ret += "\n\nLast invalid message:\n";
    ret += "len: " + String(lastInvalidMessage[3]);
    ret += "\nptr: " + String((unsigned) &lastInvalidMessage, HEX);
    ret += "\ncontent: \n";
    for (int i = 0; i < lastInvalidMessage[3]; i++) ret += String(lastInvalidMessage[i], HEX) + "-";

    ret += "\n\nResponse Buffer:\n";
    ret += "len: " + String(responseBuffer[3]);
    ret += "\nptr: " + String((unsigned) &responseBuffer, HEX);
    ret += "\ncontent: \n";
    for (int i = 0; i < responseBuffer[3]; i++) ret += String(responseBuffer[i], HEX) + "-";

    ret += "\n\nData points:\n";

    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        ret += "\n\nName: " + String(writableDataPoint[p]->getName()) + "\n";
        ret += "  Value: " + String(writableDataPoint[p]->getValue()) + "\n";
        ret += "  Send Value: " + String(writableDataPoint[p]->getSendValue(), HEX) + "\n";
        ret += "  Prevent communication: " + String(writableDataPoint[p]->preventCommunication()) + "\n";
        ret += "  Has valid value: " + String(writableDataPoint[p]->getHasValidValue()) + "\n";
        ret += "  Write pending: " + String(writableDataPoint[p]->wantsToSendValue()) + "\n";
        ret += "  Last send: " + String(m - writableDataPoint[p]->getLastSend()) + "ms ago\n";
        ret += "  Last set: " + String(m - writableDataPoint[p]->getLastSet()) + "ms ago\n";
        ret += "  Periodic send every " + String(writableDataPoint[p]->getPeriodicSend()) + "ms\n";
        ret += "  Prevent communication after " + String(writableDataPoint[p]->getPreventCommunicationAfter()) +
               "ms\n";
        p++;
    }

    server.send(200, "text/plain", ret);
}

void wifiHandleGetLog() {
    String ret = "most recent first\n";
    for (int8_t i = 0; i <MSG_LOG_SIZE; i++) {
        const char* ptr = getLastMessage(i);
        if (ptr != nullptr) {
            if (i < 10) ret += "0";
            ret += String(i) + ": " + String(ptr) + "\n";
        }
        else {
            ret += "null\n";
        }
    }
    server.send(200, "text/plain", ret);
}

void wifiHandleReadDataset() {
    if (!server.hasArg("id")) {
        server.send(500, "text/plain", "id paramater missing");
        return;
    }
    int id = server.arg("id").toInt();
    String ret = "{ ";
    Dataset *d = getDataset(id);
    if (d != nullptr) {
        ret += "\n" + String(id) + ": [";
        for (int i2 = 0; i2 < d->len; i2++) {
            ret += String(d->data[i2]);
            if (i2+1 < d->len) ret += ",";
        }
        ret += "]";
    }
    ret += "\n}";
    server.send(200, "application/json", ret);
}
void wifiHandleReadDatasets() {
    String ret = "{ ";
    for(int i = 0; i < DATASETS; i++) {
        Dataset *d = getDataset(i);
        if (d != nullptr) {
            ret += "\n" + String(i) + ": [";
            for (int i2 = 0; i2 < d->len; i2++) {
                ret += String(d->data[i2]);
                if (i2+1 < d->len) ret += ",";
            }
            ret += "],";
        }
    }
    ret += "}";
    ret.setCharAt(ret.length() - 2, '\n');
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

void wifiHandleIsConnected() {
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        server.send(200, "text/plain", "true");
    } else {
        server.send(200, "text/plain", "false");
    }
}

void wifiHandleRequestDataset() {
    if (server.hasArg("id")) requestDataset = server.arg("id").toInt();
    server.send(200, "text/plain", String(requestDataset));
}

void wifiHandleGetRequestDataset() {
    server.send(200, "text/plain", String(requestDataset));
}

void wifiHandleReboot() {
    server.send(200, "text/plain", "ok");
    delay(100);
    ESP.restart();
}

// ... setup wifi
void setupWifi() {
    wifiMgrExpose(&server);
    ElegantOTA.begin(&server);
#ifdef WIFI_SSID
    setupWifi(WIFI_SSID, WIFI_PASSWORD, WIFI_HOST);
#else
    // eeprom config
    wifiMgrPortalSetup(false, "WiFiVitotrol-", "p0rtal123");
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
    server.on("/log", wifiHandleGetLog);
    server.on("/linkState", wifiHandleGetLinkState);
    server.on("/get", wifiHandleGetData);
    server.on("/set", HTTP_POST, wifiHandleSetData);
    server.on("/isConnected", wifiHandleIsConnected);
    server.on("/reboot", wifiHandleReboot);
    server.on("/registers", wifiHandleReadRegisters);
    server.on("/dataset", wifiHandleReadDataset);
    server.on("/datasets", wifiHandleReadDatasets);
    server.on("/requestDataset", wifiHandleRequestDataset);
    server.on("/getRequestDataset", wifiHandleGetRequestDataset);

    debugPrintln("Connecting to WiFi..");
#ifdef WIFI_SSID
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    if (!MDNS.begin(WIFI_HOST)) {
        debugPrintln("Error setting up MDNS responder!");
    }
    debugPrintln(WiFi.localIP());
    server.begin();
#endif
}

void wifiLoop() {
    server.handleClient();
}
/*
async function delay(ms) {
    return new Promise(resolve => {
        setInterval(resolve, ms);
    });
}
async function waitForRequestDatasetSent() {
    while (true) {
        let r = await fetch("/getRequestDataset");
        let t = await r.text();
        if (t == "0") return;
        else await delay(500);
    }
}
async function requestDataset(id) {
    let r = await fetch("/requestDataset?id=" + encodeURIComponent(id));
}
async function requestAllDatasets() {
    let i = 0;
    while (i < 256) {
        await requestDataset(i++);
        await waitForRequestDatasetSent();
    }
}
 */