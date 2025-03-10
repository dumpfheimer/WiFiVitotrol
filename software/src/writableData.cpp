#include "writableData.h"

WritableData *writableDataPoint[WRITABLE_DATAPOINT_SIZE];


char *WritableData::getName() {
    return this->name;
}

uint16_t WritableData::getValue() {
    return this->value;
}

float WritableData::getFloatValue() {
    if (this->type == DataType::DATA_1_INT) {
        return (float) this->value;
    } else if (this->type == DataType::DATA_10_INT) {
        return ((float) this->value) / 10;
    } else {
        return 0;
    }
}

DataSource WritableData::getSource() {
    return this->source;
}

bool WritableData::getHasValidValue() {
    return this->hasValidValue;
}

unsigned long WritableData::getLastSend() {
    return this->lastSend;
}

unsigned long WritableData::getLastSet() {
    return this->lastSet;
}

unsigned long WritableData::getPreventCommunicationAfter() {
    return this->preventCommunicationAfterMS;
}

unsigned long WritableData::getPeriodicSend() {
    return this->periodicSend;
}


bool WritableData::wantsToSendValue() {
    if (!this->hasValidValue) return false;
    if (this->writePending) return true;
    if (this->periodicSend == 0) {
        return false;
    } else {
        return this->lastSend == 0 || millis() - this->lastSend > this->periodicSend;
    }
}

void WritableData::prepare() {
    this->notifyWrittenToHeater();
    this->preparationFunction();
}

void WritableData::notifyWrittenToHeater() {
    this->writePending = false;
    this->lastSend = millis();
}

uint8_t WritableData::getSendValue() {
    if (this->value > 255) {
        return 0xFF;
    } else {
        return this->value & 0xFF;
    }
}

/*void WritableData::setValueReceivedByWifi(uint16_t newVal, bool forceWrite) {
  if (this->value != newVal || forceWrite) {
    this->writePending = true;
  }
  this->value = newVal;
}
void WritableData::setValueReceivedByWifi(uint16_t newVal) {
  this->setValue(newVal, false);
}*/
void WritableData::setValueReceivedByWifi(float newVal, bool forceWrite) {
    uint16_t oldVal = this->value;
    if (this->type == DataType::DATA_1_INT) {
        this->value = (uint16_t) newVal;
    } else if (this->type == DataType::DATA_10_INT) {
        this->value = (uint16_t) (newVal * 10);
    } else {
        return;
    }
    if (this->value != oldVal || forceWrite) {
        this->writePending = true;
    }
    this->hasValidValue = true;
    this->lastSet = millis();
}

void WritableData::setValueReceivedByWifi(float newVal) {
    this->setValueReceivedByWifi(newVal, false);
}

bool WritableData::preventCommunication() {
    if (this->preventCommunicationAfterMS == 0) return false;
    if (!this->hasValidValue) return true;
    return (unsigned long) (millis() - this->lastSet) >= this->preventCommunicationAfterMS;
}

void WritableData::preventCommunicationAfter(unsigned long preventCommunicationAfterMS) {
    this->preventCommunicationAfterMS = preventCommunicationAfterMS;
}

void WritableData::init(String name, DataType t, unsigned long periodicSend, void (*preparationFunction)()) {
    this->name = (char *) malloc((name.length() + 1) * sizeof(char));
    this->type = t;
    this->periodicSend = periodicSend;
    name.toCharArray(this->name, name.length() + 1);
    this->name[name.length()] = 0;
    this->lastSend = 0;
    this->value = 0;
    this->hasValidValue = false;
    this->writePending = false;
    this->lastSet = 0;
    this->preparationFunction = preparationFunction;
    this->preventCommunicationAfterMS = (unsigned long) 0;
}

uint8_t WritableData::getSlot() {
    return this->slot;
}

void WritableData::setSlot(uint8_t slot) {
    this->slot = slot;
}

class WritableData *getWritableDataByName(String name) {
    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        //if(strcmp(writableDataPoint[p]->name, name) == 0) {
        if (name.compareTo(writableDataPoint[p]->getName()) == 0) {
            return writableDataPoint[p];
        }
        p++;
    }
    return NULL;
}

class WritableData *
createWritableDataPoint(String name, DataType t, unsigned long periodicSend, uint8_t slot, void (*preparationFunction)()) {
    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) p++;

    if (p >= WRITABLE_DATAPOINT_SIZE) {
        debugPrintln(
                "ERROR: createWritableDataPoint called, but array is full. Increase WRITABLE_DATAPOINT_SIZE");
        return NULL;
    }
    //struct WritableData *pointer = (WritableData*)malloc(sizeof(struct WritableData));
    writableDataPoint[p] = new WritableData();
    writableDataPoint[p]->init(name, t, periodicSend, preparationFunction);
    writableDataPoint[p]->setSlot(slot);
    return writableDataPoint[p];
}

bool prepareNextDataWrite(uint8_t slot) {
    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        if (writableDataPoint[p]->wantsToSendValue() && writableDataPoint[p]->getSlot() == slot) {
            writableDataPoint[p]->prepare();
            return true;
        }
        p++;
    }
    return false;
}

bool preventCommunication(uint8_t slot) {
    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        if (writableDataPoint[p]->getSlot() == slot && writableDataPoint[p]->preventCommunication()) {
            return true;
        }
        p++;
    }
    return false;
}
