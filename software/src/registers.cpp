//
// Created by chris on 24.02.23.
//

#include "registers.h"

uint8_t table[255] = {0};

void initRegisters() {
    table[0xF8] = DEVICE_CLASS;
    table[0xF9] = DEVICE_ID;
    table[0xFA] = DEVICE_SN1;
    table[0xFB] = DEVICE_SN2;
}

void setRegister(uint8_t startRegister, uint8_t len, uint8_t* start) {
    for (int i = startRegister; i < startRegister + len; i++) {
        table[i] = start[i - len];
    }
}

uint8_t* getRegisterPointer(uint8_t reg) {
    return &table[reg];
}

uint8_t getRegisterValue(uint8_t reg) {
    return table[reg];
}
#ifdef VITOCOM
uint8_t tableVitocom[255] = {0};

void initRegistersVitocom() {
    tableVitocom[0xF8] = VITOCOM_DEVICE_CLASS;
    tableVitocom[0xF9] = VITOCOM_DEVICE_ID;
    tableVitocom[0xFA] = VITOCOM_DEVICE_SN1;
    tableVitocom[0xFB] = VITOCOM_DEVICE_SN2;
}

void setRegisterVitocom(uint8_t startRegister, uint8_t len, uint8_t* start) {
    for (int i = startRegister; i < startRegister + len; i++) {
        tableVitocom[i] = start[i - len];
    }
}

uint8_t* getRegisterPointerVitocom(uint8_t reg) {
    return &tableVitocom[reg];
}

uint8_t getRegisterValueVitocom(uint8_t reg) {
    return tableVitocom[reg];
}
#endif