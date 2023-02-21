#include "wifi.h"

ESP8266WebServer server(80);

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
  DEBUG_SERIAL.println("Requested URI ");
  DEBUG_SERIAL.println(uri);
  DEBUG_SERIAL.println(datapointName);

  WritableData* datapoint = getWritableDataByName(datapointName);

  if (datapoint == NULL) {
    // TODO: iterate over read-only-datapoints
    server.send(404, "text/plain", "NOT FOUND");
  }

  server.send(200, "text/plain", String(datapoint->getFloatValue()));
}
void wifiHandleSetData() {
  String uri = server.uri();
  String datapointName = uri.substring(4);
  DEBUG_SERIAL.println("Requested URI ");
  DEBUG_SERIAL.println(uri);
  DEBUG_SERIAL.println(datapointName);

  WritableData* datapoint = getWritableDataByName(datapointName);

  if (datapoint == NULL) {
    server.send(404, "text/plain", "NOT FOUND");
  }
  
  String strValue;
  float floatValue;
  bool hasValue = false;
  bool forceWrite = false;
  for (uint8_t i = 0; i < server.args(); i++) {
    if(server.argName(i) == "val") {
      strValue = server.arg(i).c_str();
      floatValue = strValue.toFloat();
      hasValue = true;
      break;
    }
    if(server.argName(i) == "force" || server.argName(i) == "forceWrite") {
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
  DEBUG_SERIAL.println("Requested URI ");
  DEBUG_SERIAL.println(uri);
  DEBUG_SERIAL.println(datapointName);

  WritableData* datapoint = getWritableDataByName(datapointName);

  if (datapoint == NULL) {
    // TODO: iterate over read-only-datapoints
    server.send(404, "text/plain", "NOT FOUND");
  }

  switch(datapoint->getSource()) {
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
  if (lastHeaterCommandReceivedAt > 0 && lastHeaterCommandReceivedAt + 60000 >= millis()) {
    ret += "Heater is connected\r\n";
  } else {
    ret += "Heater is not connected\r\n";
  }
  if (preventCommunication()) {
    ret += "Communication is prevented\r\n";
  } else {
    ret += "Communication is not prevented\r\n";
  }
  ret += "Data points:\r\n";
  
  uint8_t p = 0;
  while (writableDataPoint[p] != NULL) {
    ret += "\r\n\r\nName: " + String(writableDataPoint[p]->getName()) + "\r\n";
    ret += "  Value: " + String(writableDataPoint[p]->getValue()) + "\r\n";
    ret += "  Send Value: " + String(writableDataPoint[p]->getSendValue(), HEX) + "\r\n";
    ret += "  Prevent communication: " + String(writableDataPoint[p]->preventCommunication()) + "\r\n";
    ret += "  Has valid value: " + String(writableDataPoint[p]->getHasValidValue()) + "\r\n";
    ret += "  Write pending: " + String(writableDataPoint[p]->wantsToSendValue()) + "\r\n";
    ret += "  Last send: " + String(millis() - writableDataPoint[p]->getLastSend()) + "ms ago\r\n";
    ret += "  Last set: " + String(millis() - writableDataPoint[p]->getLastSet()) + "ms ago\r\n";
    ret += "  Periodic send every " + String(writableDataPoint[p]->getPeriodicSend()) + "ms\r\n";
    ret += "  Prevent communication after " + String(writableDataPoint[p]->getPreventCommunicationAfter()) + "ms\r\n";
    p++;
  }
  
  server.send(200, "text/plain", ret);
}
void wifiHandleIsPreventCommunication() {
  if (preventCommunication()) {
    server.send(200, "text/plain", "true");
  } else {
    server.send(200, "text/plain", "false");
  }
}
void wifiHandleIsConnected() {
  if (lastHeaterCommandReceivedAt > 0 && lastHeaterCommandReceivedAt + 60000 >= millis()) {
    server.send(200, "text/plain", "true");
  } else {
    server.send(200, "text/plain", "false");
  }
}
void wifiHandleReboot() {
  server.send(200, "text/plain", "ok");
  delay(100);
  ESP.restart();
}
// ... setup wifi
void setupWifi() {
    WiFi.begin(wifiSSID, wifiPassword);
    WiFi.setAutoConnect(true);
#ifdef ESP32
    WiFi.setHostname(wifiHost);
#endif

    uint8_t p = 0;
    while (writableDataPoint[p] != NULL) {
        server.on("/get" + String(writableDataPoint[p]->getName()), wifiHandleGetData);
        server.on("/set" + String(writableDataPoint[p]->getName()), HTTP_POST, wifiHandleSetData);
        server.on("/getSource" + String(writableDataPoint[p]->getName()), wifiHandleGetSource);
        DEBUG_SERIAL.print("dynamically created handler /get- and /set");
        DEBUG_SERIAL.println(String(writableDataPoint[p]->getName()));
        p++;
    }

    server.on("/", wifiHandleGetOverview);
    server.on("/get", wifiHandleGetData);
    server.on("/set", HTTP_POST, wifiHandleSetData);
    server.on("/isConnected", wifiHandleIsConnected);
    server.on("/reboot", wifiHandleReboot);

    DEBUG_SERIAL.println("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    if (!MDNS.begin(wifiHost)) {
        DEBUG_SERIAL.println("Error setting up MDNS responder!");
    }
    DEBUG_SERIAL.println(WiFi.localIP());

    ElegantOTA.begin(&server);

    server.begin();
}
void wifiLoop() {
    server.handleClient();
}