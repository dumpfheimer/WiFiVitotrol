#include "messageHandler.h"

#define MSG_PING 0x00
#define MSG_READ_REQUEST_1 0x31
#define MSG_READ_REQUEST_N 0x33
#define MSG_READ_REQUEST_DATASET 0x3F
#define MSG_PONG 0x80
#define MSG_WRITE_DATA_1 0xB1
#define MSG_WRITE_DATA_N 0xB3
#define MSG_WRITE_DATASET 0xBF

bool workDataWriteOrPing() {
  if(!prepareNextDataWrite()) {
    byte ret[1];
    ret[0] = 0x80;
    prepareResponse(0x08, ret, 1);
  }
  return true;
}

bool workRequestN(uint8_t addr, uint8_t len) {
  uint8_t retLen = 0;
  byte ret[8];
  switch (addr) {
    case 0xF8:
      retLen = 8;
      ret[0] = 0xF8;
      ret[1] = DEVICE_CLASS;
      ret[2] = 0xF9;
      ret[3] = DEVICE_ID;
      ret[4] = 0xFA;
      ret[5] = DEVICE_SN1;
      ret[6] = 0xFB;
      ret[7] = DEVICE_SN2;
    break;
    default:
      DEBUG_SERIAL.print("invalid n address requested: ");
      DEBUG_SERIAL.println(addr);
      return false;
    break;
  }
  prepareResponse(0xB3, ret, retLen);
  return true;
}

bool workRequest1(uint8_t addr) {
  DEBUG_SERIAL.println("workRequest1 RECEIVED BUT NOT HANDLED");
  byte ret[1];
  
  switch (addr) {
    case 0xF8:
      ret[0] = 0x11;
    break;
    default:
      DEBUG_SERIAL.print("invalid n address requested: ");
      DEBUG_SERIAL.println(addr);
      return false;
    break;
  }
  
  prepareResponse(0xB1, ret, 1);
  return true;
}

bool workWriteDataset(byte buffer[], uint8_t bufferLength) {
  byte writeType = buffer[0];

  switch(writeType) {
    case 0x1D:
      for(uint8_t i = 1; i < bufferLength; i++) buffer[i] = buffer[i] ^ 0xAA;
      return false;
    default:
      return false;
  }
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

    byte *msg = buff + 6;
    uint8_t msgLen = len - 6 - 2; // 6=header 2=crc

    if (cmd != MSG_READ_REQUEST_N || msg[0] != 0xF8) {
        lastHeaterCommandReceivedAt = millis();
    }

    switch(cmd) {
        case MSG_PING:
            return workDataWriteOrPing();
        case MSG_READ_REQUEST_1:
            return workRequest1(buff[6]);
        case MSG_READ_REQUEST_N:
            return workRequestN(buff[6], buff[7]);
        case MSG_WRITE_DATASET:
            return workWriteDataset(msg, msgLen);
        default:
            DEBUG_SERIAL.println("not sure how to handle message:");
            printAsHex(&DEBUG_SERIAL, buff, buffLen);
            DEBUG_SERIAL.println();
            return false;
    }
}