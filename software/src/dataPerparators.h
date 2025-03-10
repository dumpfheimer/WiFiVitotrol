//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATAPERPARATORS_H
#define SOFTWARE_DATAPERPARATORS_H

#include <Arduino.h>
#include "dataConfiguration.h"
#include "responseHelper.h"

void prepareSendCurrentRoomTemperature();
#ifdef ENABLE_SLOT_2
void prepareSendCurrentRoomTemperatureH2();
#endif
#ifdef ENABLE_SLOT_3
void prepareSendCurrentRoomTemperatureH3();
#endif
void prepareSendDesiredRoomTemperature();

#endif //SOFTWARE_DATAPERPARATORS_H
