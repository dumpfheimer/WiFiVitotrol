//
// Created by chris on 24.02.23.
//

#ifndef SOFTWARE_DATASET_VITOCOM_H
#define SOFTWARE_DATASET_VITOCOM_H

#include <Arduino.h>
#include "dataset.h"

void setDatasetVitocom(uint8_t datasetId, const byte* dataset, uint8_t length);
Dataset* getDatasetVitocom(uint8_t datasetId);

#endif //SOFTWARE_DATASET_H
