//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_RESPONSEHELPER_H
#define SOFTWARE_RESPONSEHELPER_H

#include <Arduino.h>
#include "main.h"

void clearResponseBuffer();
void addCRCToBuffer(uint8_t bufferLength);
void prepareResponse(uint8_t cmd, byte msg[], uint8_t msgLen);
void sendResponse();

#endif //SOFTWARE_RESPONSEHELPER_H
