//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MESSAGEHANDLER_H
#define SOFTWARE_MESSAGEHANDLER_H

#include <Arduino.h>

#include "writableData.h"
#include "responseHelper.h"


//        VITOTROL CONFIGURATION
#define DEVICE_CLASS 0x11 // from https://github.com/boblegal31/Heater-remote/blob/1a857b10db96405937f65ed8338e314f57adaedf/NetRemote/example/inc/ViessMann.h
#define DEVICE_SLOT 0x01

#define DEVICE_ID 0x34
#define DEVICE_SN1 0x00
#define DEVICE_SN2 0x05


bool workMessageAndCreateResponseBuffer(byte buff[], uint8_t buffLen);

#endif //SOFTWARE_MESSAGEHANDLER_H
