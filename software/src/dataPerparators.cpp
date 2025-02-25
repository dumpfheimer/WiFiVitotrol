#include "dataPerparators.h"

// the current room temperature
void prepareSendCurrentRoomTemperature() {
    // send current room temperature
    byte ret[4];
    ret[0] = 0x20;
    if (currentRoomTemperature->getFloatValue() > 25.5) {
        ret[1] = ((int) currentRoomTemperature->getValue() - 256) ^ 0xAA;
        ret[2] = 0x01 ^ 0xAA;
    } else {
        ret[1] = currentRoomTemperature->getSendValue() ^ 0xAA;
        ret[2] = 0x00 ^ 0xAA;
    }
    ret[3] = 0x00 ^ 0xAA;
    prepareResponse(0xBF, ret, 4, DEVICE_CLASS, DEVICE_SLOT);
}

// the desired room temperature
void prepareSendDesiredRoomTemperature() {
    // send desired room temperature
    byte ret[6];
    ret[0] = 0x15; // 14 Command / Table
    ret[1] = 0x0C ^ 0xAA; // AA ignored?!
    ret[2] = 0x01 ^ 0xAA; // AB Heizkreis?!
    ret[3] = 0xCD ^ 0xAA; // 67 CD=normal room temperature CE=reduced room temperature
    ret[4] = desiredRoomTemperature->getSendValue() ^ 0xAA; // the temperature
    ret[5] = 0x00 ^ 0xAA;
    prepareResponse(0xBF, ret, 6, DEVICE_CLASS, DEVICE_SLOT);
}