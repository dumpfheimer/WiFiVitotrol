//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MESSAGEHANDLER_H
#define SOFTWARE_MESSAGEHANDLER_H

#include <Arduino.h>

#include "writableData.h"
#include "responseHelper.h"
#include "registers.h"
#include "dataset.h"

bool workMessageAndCreateResponseBuffer(byte buff[], uint8_t buffLen);

#endif //SOFTWARE_MESSAGEHANDLER_H
