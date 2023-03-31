//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MAIN_H
#define SOFTWARE_MAIN_H

#include <Arduino.h>
#include <ElegantOTA.h>
#include "dataEnums.h"
#include "configuration.h"

//        SERIAL CONFIGURATION
#ifdef ESP8266
#include "SoftwareSerial.h"
#endif

#include "utils.h"
#include "responseHelper.h"
#include "dataConfiguration.h"
#include "wifi.h"
#include "messageHandler.h"
#include "crc.h"
#include "writableData.h"
#include "utils.h"
#include "registers.h"
#include "dataset.h"

//        BUFFER CONFIGURATION
// stick to 255 to prevent uint8 of overflowing
#define BUFFER_LEN 255

extern const char *wifiSSID;
extern const char *wifiPassword;
extern const char *wifiHost;
extern const long rebootTimeout;

extern byte buffer[BUFFER_LEN];
extern byte lastValidMessage[BUFFER_LEN];
extern byte lastInvalidMessage[BUFFER_LEN];
extern byte responseBuffer[BUFFER_LEN];
extern uint8_t bufferPointer;
extern unsigned long lastHeaterCommandReceivedAt;
extern unsigned long lastReadAt;
extern unsigned long lastCommandReceivedAt;
extern unsigned long lastMessageAt;
extern unsigned long lastValidMessageAt;
extern unsigned long lastMessageWithResponseAt;
extern unsigned long lastMessageWithoutResponseAt;
extern byte readByte;
extern uint8_t requestDataset;
extern unsigned long lastBroadcastMessage;

void serialLoopForever(void *pvParameters);

#endif //SOFTWARE_MAIN_H
