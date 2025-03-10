//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATAPERPARATORS_H
#define SOFTWARE_DATAPERPARATORS_H

#include <Arduino.h>
#include "dataConfiguration.h"
#include "responseHelper.h"

#ifdef ENABLE_SLOT_1
void prepareSendCurrentRoomTemperature();
void prepareSendDesiredRoomTemperature();
#endif
#ifdef ENABLE_SLOT_2
void prepareSendCurrentRoomTemperatureH2();
void prepareSendDesiredRoomTemperatureH2();
#endif
#ifdef ENABLE_SLOT_3
void prepareSendCurrentRoomTemperatureH3();
void prepareSendDesiredRoomTemperatureH3();
#endif

#endif //SOFTWARE_DATAPERPARATORS_H
