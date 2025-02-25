//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MESSAGEHANDLER_VITOCOM_H
#define SOFTWARE_MESSAGEHANDLER_VITOCOM_H

#include <Arduino.h>

#include "writableData.h"
#include "responseHelper.h"
#include "registers.h"
#include "datasetVitocom.h"

bool workMessageAndCreateResponseBufferVitocom(byte buff[]);

#endif //SOFTWARE_MESSAGEHANDLER_H
