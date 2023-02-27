#include "messageHandler.h"

#define MSG_PING 0x00
#define MSG_MASTER_REQUESTING_REQUEST_1 0x31
#define MSG_MASTER_REQUESTING_REQUEST_N 0x33
#define MSG_MASTER_REQUESTING_REQUEST_DATASET 0x3F
#define MSG_PONG 0x80
#define MST_MASTER_SENDING_DATA_1 0xB1
#define MST_MASTER_SENDING_DATA_N 0xB3
#define MST_MASTER_SENDING_DATASET 0xBF

byte requestedDataResponseBuffer[512] = {0};

bool workPing() {
    if (!prepareNextDataWrite()) {
        requestedDataResponseBuffer[0] = 0x80;
        prepareResponse(0x08, requestedDataResponseBuffer, 1);
    }
    return true;
}

bool workMasterRequestedN(uint8_t addr, uint8_t len) {
    for (int x = 0; x < len; x++) {
        requestedDataResponseBuffer[2*x] = addr + x;
        requestedDataResponseBuffer[2*x+1] = getRegisterValue(addr + x);
    }
    prepareResponse(0xB3, requestedDataResponseBuffer, len * 2);
    return true;
}

bool workMasterRequested1(uint8_t addr) {
    requestedDataResponseBuffer[0] = addr;
    requestedDataResponseBuffer[1] = getRegisterValue(addr);

    prepareResponse(0xB1, requestedDataResponseBuffer, 2);
    return true;
}

bool workMasterSentDataset(byte message[], uint8_t messageLength) {
    for (int i = 1; i < messageLength; i++) message[i] = message[i] ^ 0xAA;
    setDataset(message[0], &message[1], messageLength - 1);

    return true;
}

bool workMasterSent1(uint8_t addr, uint8_t value) {
    setRegister(addr, 1, &value);
    return true;
}

bool workMasterSentN(byte buffer[]) {
    int regCount = buffer[0];
    for (int x = 0; x < regCount; x++) {
        setRegister(buffer[1+2*x], 1, &buffer[2+2*x]);
    }
    return true;
}

// will return whether or not the message could be processed
bool workMessageAndCreateResponseBuffer(byte buff[], uint8_t buffLen) {
    uint8_t destinationClass = buff[0];
    uint8_t sourceClass = buff[1];
    uint8_t cmd = buff[2];
    uint8_t len = buff[3];
    uint8_t dsl = buff[4];
    uint8_t ssk = buff[5];

    if (destinationClass != 0x11) return false;
    if (sourceClass != 0x00) return false;

    byte *msg = &buff[6];
    uint8_t msgLen = len - 6 - 2; // 6=header 2=crc

    if (cmd != MSG_MASTER_REQUESTING_REQUEST_N || msg[0] != 0xF8) {
        lastHeaterCommandReceivedAt = millis();
    }

    switch (cmd) {
        case MSG_PING:
            return workPing();
        case MSG_MASTER_REQUESTING_REQUEST_1:
            return workMasterRequested1(buff[6]);
        case MSG_MASTER_REQUESTING_REQUEST_N:
            return workMasterRequestedN(msg[0], msg[1]);
        case MST_MASTER_SENDING_DATA_1:
            return workMasterSent1(msg[0], msg[1]);
        case MST_MASTER_SENDING_DATA_N:
            return workMasterSentN(msg);
        case MST_MASTER_SENDING_DATASET:
            //return false;
            return workMasterSentDataset(msg, msgLen);
        default:
#if DEBUG == true
            debugPrintln("not sure how to handle message:");
            printAsHex(&DebugSerial, buff, buffLen);
            debugPrintln("");
#endif
            return false;
    }
}