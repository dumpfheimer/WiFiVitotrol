//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATACONFIGURATION_H
#define SOFTWARE_DATACONFIGURATION_H

#include "configuration.h"
#include "dataEnums.h"
#include "dataPerparators.h"

extern class WritableData *currentRoomTemperature;

extern class WritableData *desiredRoomTemperature;

void configureData();

#endif //SOFTWARE_DATACONFIGURATION_H
