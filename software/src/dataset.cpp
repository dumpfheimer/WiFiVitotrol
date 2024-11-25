//
// Created by chris on 24.02.23.
//

#include "dataset.h"

Dataset* datasets[DATASETS] = {nullptr};

void setDataset(uint8_t datasetId, const byte* dataset, uint8_t length) {
    if (datasetId >= DATASETS) return;
    if (datasets[datasetId] != nullptr) {
        if (datasets[datasetId]->data != nullptr) free(datasets[datasetId]->data);
        free(datasets[datasetId]);
    }
    datasets[datasetId] = (Dataset*) malloc(sizeof(Dataset));
    if (datasets[datasetId] != nullptr) {
        datasets[datasetId]->len = length;
        datasets[datasetId]->data = (uint8_t *) malloc(sizeof(uint8_t) * length);
        if (datasets[datasetId]->data != nullptr) {
            for (int i = 0; i < length; i++) datasets[datasetId]->data[i] = dataset[i];
        } else {
            free(datasets[datasetId]);
        }
    }
}
Dataset* getDataset(uint8_t datasetId) {
    if (datasetId >= DATASETS) return nullptr;
    return datasets[datasetId];
}