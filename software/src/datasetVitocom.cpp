//
// Created by chris on 24.02.23.
//

#include "datasetVitocom.h"

Dataset* datasetsVitocom[DATASETS] = {nullptr};

void setDatasetVitocom(uint8_t datasetId, const byte* dataset, uint8_t length) {
    if (datasetId >= DATASETS) return;
    if (datasetsVitocom[datasetId] != nullptr) {
        if (datasetsVitocom[datasetId]->data != nullptr) free(datasetsVitocom[datasetId]->data);
        free(datasetsVitocom[datasetId]);
    }
    datasetsVitocom[datasetId] = (Dataset*) malloc(sizeof(Dataset));
    if (datasetsVitocom[datasetId] != nullptr) {
        datasetsVitocom[datasetId]->len = length;
        datasetsVitocom[datasetId]->data = (uint8_t *) malloc(sizeof(uint8_t) * length);
        if (datasetsVitocom[datasetId]->data != nullptr) {
            for (int i = 0; i < length; i++) datasetsVitocom[datasetId]->data[i] = dataset[i];
        } else {
            free(datasetsVitocom[datasetId]);
        }
    }
}
Dataset* getDatasetVitocom(uint8_t datasetId) {
    if (datasetId >= DATASETS) return nullptr;
    return datasetsVitocom[datasetId];
}