//
// Created by chris on 24.02.23.
//

#ifndef SOFTWARE_REGISTERS_H
#define SOFTWARE_REGISTERS_H

#include <Arduino.h>
#include "configuration.h"

void initRegisters();
void setRegister(uint8_t startRegister, uint8_t len, uint8_t* start);
uint8_t* getRegisterPointer(uint8_t reg);
uint8_t getRegisterValue(uint8_t reg);

#endif //SOFTWARE_REGISTERS_H
