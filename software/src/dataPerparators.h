//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATAPERPARATORS_H
#define SOFTWARE_DATAPERPARATORS_H

#include <Arduino.h>
#include "dataConfiguration.h"
#include "responseHelper.h"

void prepareSendCurrentRoomTemperature();
void prepareSendCurrentRoomTemperatureH2();
void prepareSendDesiredRoomTemperature();

#endif //SOFTWARE_DATAPERPARATORS_H
