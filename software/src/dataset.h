//
// Created by chris on 24.02.23.
//

#ifndef SOFTWARE_DATASET_H
#define SOFTWARE_DATASET_H

#include <Arduino.h>
#define DATASETS 32

struct Dataset {
    uint8_t len;
    uint8_t *data;
};

void setDataset(uint8_t datasetId, byte* dataset, uint8_t length);
Dataset* getDataset(uint8_t datasetId);

#endif //SOFTWARE_DATASET_H
