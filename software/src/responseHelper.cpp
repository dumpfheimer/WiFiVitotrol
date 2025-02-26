#include "responseHelper.h"

// overwrites the response buffer with zeros
void clearResponseBuffer() {
    for (int i = 0; i < BUFFER_LEN; i++) responseBuffer[i] = 0;
}

// calculate the checksum and append it to buffer
void addCRCToBuffer(uint8_t bufferLength) {
    uint16_t crc = calculateViessmannCRC(responseBuffer, bufferLength);
    uint8_t b2 = crc & 0xFF;
    uint8_t b1 = (crc >> 8) & 0xFF;

    responseBuffer[bufferLength] = b1;
    responseBuffer[bufferLength + 1] = b2;
}

void prepareResponse(uint8_t cmd, const byte msg[], uint8_t msgLen) {
    clearResponseBuffer();
    responseBuffer[0] = 0x00; // dest class
    responseBuffer[1] = DEVICE_CLASS; // source class
    responseBuffer[2] = cmd;  // cmd
    responseBuffer[3] = 8 + msgLen;  // length
    responseBuffer[4] = DEVICE_SLOT;
    responseBuffer[5] = 0x01;

    // copy msg to responseBuffer
    for (uint8_t i = 0; i < msgLen && i < BUFFER_LEN - 8; i++) {
        responseBuffer[i + 6] = msg[i];
    }

    // calculate crc
    addCRCToBuffer(msgLen + 6); // first empty slot after message
}

void sendResponse() {
    uint8_t startAtIndex = 0;
    if ((millis() -lastMessageAt) < 20) delay(millis() -lastMessageAt);

    if (isValidCRC(responseBuffer, responseBuffer[3])) {
        pushLastMessage('w', responseBuffer, responseBuffer[3]);
    } else {
        uint8_t len = responseBuffer[3] - 2;
        addCRCToBuffer(len);
        pushLastMessage('W', responseBuffer, responseBuffer[3]);
    }

    for (uint8_t i = startAtIndex; i < responseBuffer[3] && i < BUFFER_LEN; i++) {
        while (!ModbusSerial.availableForWrite()) yield();
        ModbusSerial.write(responseBuffer[i]);
    }

    ModbusSerial.flush();
}
