//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATACONFIGURATION_H
#define SOFTWARE_DATACONFIGURATION_H

#include "configuration.h"
#include "dataEnums.h"
#include "dataPerparators.h"

extern class WritableData *currentRoomTemperature;

#ifdef ENABLE_SLOT_2
extern class WritableData *currentRoomTemperatureH2;
#endif

#ifdef ENABLE_SLOT_3
extern class WritableData *currentRoomTemperatureH3;
#endif

extern class WritableData *desiredRoomTemperature;

void configureData();

#endif //SOFTWARE_DATACONFIGURATION_H
