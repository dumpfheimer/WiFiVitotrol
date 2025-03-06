//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MAIN_H
#define SOFTWARE_MAIN_H

#include <Arduino.h>
#include <ElegantOTA.h>
#include "dataEnums.h"
#include "configuration.h"
#include "mqtt.h"

//        SERIAL CONFIGURATION
#ifdef ESP8266
#include "SoftwareSerial.h"
#endif

#ifdef FIND_DEVICE_ID
extern bool deviceIdFound;
#endif

#include "utils.h"
#include "responseHelper.h"
#include "dataConfiguration.h"
#include "vito_wifi.h"
#include "messageHandler.h"
#include "crc.h"
#include "writableData.h"
#include "utils.h"
#include "registers.h"
#include "dataset.h"

//        BUFFER CONFIGURATION
// stick to 255 to prevent uint8 of overflowing
#define BUFFER_LEN 255
#define LINK_STATE_LENGTH 64

#ifdef WIFI_SSID
#ifndef WIFI_PASSWORD
#error "if WIFI_SSID is defined, WIFI_PASSWORD needs to be defined too"
#endif
#ifndef WIFI_HOST
#error "if WIFI_SSID is defined, WIFI_HOST needs to be defined too"
#endif
#endif
extern const long rebootTimeout;
extern char* linkState;


void pushLastMessage(char* msg);
void pushLastMessage(char prefix, byte* buff, uint8_t size);
uint8_t getLastMessageSize();
const char* getLastMessage(uint8_t n);

uint8_t getSlotCount();
uint8_t getSlot(uint8_t n);
void addSlot(uint8_t slot);

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
extern uint8_t requestByte;
extern unsigned long lastBroadcastMessage;

void notifyCommandReceived();
const char* getLinkState();
void setLinkState(const char *newState);

#endif //SOFTWARE_MAIN_H
