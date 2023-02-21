//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MAIN_H
#define SOFTWARE_MAIN_H

#include <Arduino.h>
#include "dataEnums.h"
#include "configuration.h"
#include <ElegantOTA.h>

//        SERIAL CONFIGURATION
#ifdef ESP8266
#include "SoftwareSerial.h"

#endif
#ifdef ESP32
#define OUT_SERIAL Serial2
#define IN_SERIAL Serial2
#define DEBUG_SERIAL Serial
#define THREADED true
#endif

#include "utils.h"
#include "responseHelper.h"
#include "dataConfiguration.h"
#include "wifi.h"
#include "messageHandler.h"
#include "crc.h"
#include "writableData.h"

//        BUFFER CONFIGURATION
// stick to 255 to prevent uint8 of overflowing
#define BUFFER_LEN 255

extern const char* wifiSSID;
extern const char* wifiPassword;
extern const char* wifiHost;
extern const long rebootTimeout;

extern byte buffer[BUFFER_LEN];
extern byte responseBuffer[BUFFER_LEN];
extern uint8_t bufferPointer;
extern unsigned long lastHeaterCommandReceivedAt;
extern unsigned long lastCommandReceivedAt;

#endif //SOFTWARE_MAIN_H