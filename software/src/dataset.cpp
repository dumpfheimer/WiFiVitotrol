//
// Created by chris on 24.02.23.
//

#include "dataset.h"

Dataset* datasets[DATASETS] = {nullptr};

void setDataset(uint8_t datasetId, const byte* dataset, uint8_t length) {
    if (datasetId >= DATASETS) return;
    if (datasets[datasetId]) {
        if (datasets[datasetId]->data) free(datasets[datasetId]->data);
        free(datasets[datasetId]);
    }
    datasets[datasetId] = (Dataset*) malloc(sizeof(Dataset));
    if (datasets[datasetId]) {
        datasets[datasetId]->len = length;
        datasets[datasetId]->data = (uint8_t *) malloc(sizeof(uint8_t) * length);
        if (datasets[datasetId]->data) {
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
bool getOutsideTemp(double* temp) {
    Dataset *d = getDataset(29);
    if (d == nullptr) return false;
    *temp = (int8_t) d->data[6];
    return true;
}
bool isHeatingEnabled(bool* isHeating) {
    // or pump running?
    Dataset *d = getDataset(29);
    if (d == nullptr) return false;
    byte b = d->data[10];
    *isHeating = (b & 0b1000000) > 0;
    return true;
}
bool isHeating(bool* isHeating) {
    // 1x warmwasserbereitung
    Dataset *d = getDataset(20);
    if (d == nullptr) return false;
    byte b = d->data[4];
    *isHeating =  b & 0b1;
    return true;
}
bool is1xWWheating(bool* is1xWW) {
    // 1x warmwasserbereitung
    Dataset *d = getDataset(20);
    if (d == nullptr) return false;
    byte b = d->data[7];
    *is1xWW =  b & 0b1;
    return true;
}
bool isWWheating(bool* isWW) {
    // 1x warmwasserbereitung
    Dataset *d = getDataset(20);
    if (d == nullptr) return false;
    byte b = d->data[6];
    *isWW =  b & 0b1;
    return true;
}