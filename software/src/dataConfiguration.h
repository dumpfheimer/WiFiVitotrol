//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_DATACONFIGURATION_H
#define SOFTWARE_DATACONFIGURATION_H

#include "configuration.h"
#include "dataEnums.h"
#include "dataPerparators.h"

#ifdef ENABLE_SLOT_1
extern class WritableData *currentRoomTemperature;
extern class WritableData *desiredRoomTemperature;
#endif

#ifdef ENABLE_SLOT_2
extern class WritableData *currentRoomTemperatureH2;
extern class WritableData *desiredRoomTemperatureH2;
#endif

#ifdef ENABLE_SLOT_3
extern class WritableData *currentRoomTemperatureH3;
extern class WritableData *desiredRoomTemperatureH3;
#endif


void configureData();

#endif //SOFTWARE_DATACONFIGURATION_H
