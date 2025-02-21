//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_WIFI_H
#define SOFTWARE_WIFI_H

#include "configuration.h"
#include "dataEnums.h"
#include "writableData.h"
#include "main.h"
#include "dataset.h"
#include "wifi_mgr.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

extern ESP8266WebServer server;
#endif

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#endif


void setupWifi();
void wifiLoop();

#endif //SOFTWARE_WIFI_H
