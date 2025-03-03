//
// Created by chris on 24.02.23.
//

#include "registers.h"

uint8_t vitotrolRegisters[256] = {0};

void initRegisters() {
    vitotrolRegisters[0x00] = 0x12;
    vitotrolRegisters[0xF8] = DEVICE_CLASS;
    vitotrolRegisters[0xF9] = DEVICE_ID;
    vitotrolRegisters[0xFA] = DEVICE_SN1;
    vitotrolRegisters[0xFB] = DEVICE_SN2;
}

void setRegister(uint8_t startRegister, uint8_t len, uint8_t* start) {
    for (int i = startRegister; i < startRegister + len; i++) {
        vitotrolRegisters[i] = start[i - len];
    }
}

uint8_t* getRegisterPointer(uint8_t reg) {
    return &vitotrolRegisters[reg];
}

uint8_t getRegisterValue(uint8_t reg) {
    return vitotrolRegisters[reg];
}