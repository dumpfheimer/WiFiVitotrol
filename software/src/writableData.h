//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_WRITABLEDATA_H
#define SOFTWARE_WRITABLEDATA_H

#include <Arduino.h>

#include "configuration.h"
#include "dataEnums.h"
#include "utils.h"

class WritableData {
    char *name = NULL;
    uint8_t slot = 1;
    uint16_t value = 0;
    DataSource source;
    DataType type;
    bool hasValidValue = false;
    bool writePending = false;
    unsigned long periodicSend = 0;
    unsigned long lastSend = 0;
    unsigned long lastSet = 0;
    unsigned long preventCommunicationAfterMS = 0;

    void (*preparationFunction)() = NULL;

public:
    char *getName();

    uint16_t getValue();

    float getFloatValue();

    DataSource getSource();

//  DataType getType();
    bool getHasValidValue();

    unsigned long getLastSend();

    unsigned long getLastSet();

    unsigned long getPreventCommunicationAfter();

    unsigned long getPeriodicSend();

    bool wantsToSendValue();

    void setValueReceivedByWifi(float value);

    void setValueReceivedByWifi(float value, bool forceWrite);
//  void setValueReceivedByWifi(uint16_t value);
//  void setValueReceivedByWifi(uint16_t value, bool forceWrite);
//  void setValueReceivedFromHeater(uint16_t value);

    void prepare();

    void notifyWrittenToHeater();

    uint8_t getSendValue();

    bool preventCommunication();

    void preventCommunicationAfter(unsigned long preventCommunicationAfterMS);

    void init(String name, DataType t, unsigned long periodicSend, void (*perparationFunction)());

    uint8_t getSlot();

    void setSlot(uint8_t slot);
};

extern WritableData *writableDataPoint[WRITABLE_DATAPOINT_SIZE];

bool preventCommunication(uint8_t slot);

class WritableData *getWritableDataByName(String name);

class WritableData *
createWritableDataPoint(String name, DataType t, unsigned long periodicSend, uint8_t slot, void (*preparationFunction)());

bool prepareNextDataWrite(uint8_t slot);

#endif //SOFTWARE_WRITABLEDATA_H
