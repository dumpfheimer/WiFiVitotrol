#include "dataPerparators.h"

#ifdef ENABLE_SLOT_1
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
    prepareResponse(MSG_SENDING_COMMAND, ret, 4, 0x01);
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
    prepareResponse(MSG_SENDING_COMMAND, ret, 6, 0x01);
}
#endif

#ifdef ENABLE_SLOT_2
// the current room temperature
void prepareSendCurrentRoomTemperatureH2() {
    // send current room temperature
    byte ret[4];
    ret[0] = 0x21;
    if (currentRoomTemperatureH2->getFloatValue() > 25.5) {
        ret[1] = ((int) currentRoomTemperatureH2->getValue() - 256) ^ 0xAA;
        ret[2] = 0x01 ^ 0xAA;
    } else {
        ret[1] = currentRoomTemperatureH2->getSendValue() ^ 0xAA;
        ret[2] = 0x00 ^ 0xAA;
    }
    ret[3] = 0x00 ^ 0xAA;
    prepareResponse(MSG_SENDING_COMMAND, ret, 4, 0x02);
}

// the desired room temperature
void prepareSendDesiredRoomTemperatureH2() {
    // send desired room temperature
    byte ret[6];
    ret[0] = 0x16; // 14 Command / Table
    ret[1] = 0x0C ^ 0xAA; // AA ignored?!
    ret[2] = 0x01 ^ 0xAA; // AB Heizkreis?!
    ret[3] = 0xCD ^ 0xAA; // 67 CD=normal room temperature CE=reduced room temperature
    ret[4] = desiredRoomTemperatureH2->getSendValue() ^ 0xAA; // the temperature
    ret[5] = 0x00 ^ 0xAA;
    prepareResponse(MSG_SENDING_COMMAND, ret, 6, 0x02);
}
#endif

#ifdef ENABLE_SLOT_3
// the current room temperature
void prepareSendCurrentRoomTemperatureH3() {
    // send current room temperature
    byte ret[4];
    ret[0] = 0x22;
    if (currentRoomTemperatureH3->getFloatValue() > 25.5) {
        ret[1] = ((int) currentRoomTemperatureH3->getValue() - 256) ^ 0xAA;
        ret[2] = 0x01 ^ 0xAA;
    } else {
        ret[1] = currentRoomTemperatureH3->getSendValue() ^ 0xAA;
        ret[2] = 0x00 ^ 0xAA;
    }
    ret[3] = 0x00 ^ 0xAA;
    prepareResponse(MSG_SENDING_COMMAND, ret, 4, 0x02);
}
// the desired room temperature
void prepareSendDesiredRoomTemperatureH3() {
    // send desired room temperature
    byte ret[6];
    ret[0] = 0x17; // 14 Command / Table
    ret[1] = 0x0C ^ 0xAA; // AA ignored?!
    ret[2] = 0x01 ^ 0xAA; // AB Heizkreis?!
    ret[3] = 0xCD ^ 0xAA; // 67 CD=normal room temperature CE=reduced room temperature
    ret[4] = desiredRoomTemperatureH3->getSendValue() ^ 0xAA; // the temperature
    ret[5] = 0x00 ^ 0xAA;
    prepareResponse(MSG_SENDING_COMMAND, ret, 6, 0x03);
}
#endif
