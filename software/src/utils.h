//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_UTILS_H
#define SOFTWARE_UTILS_H

#include <Arduino.h>
#include "SoftwareSerial.h"

#include "main.h"

#ifdef ESP8266
void printAsHex(SoftwareSerial* serial, byte *array, unsigned int len);
#endif

void printAsHex(HardwareSerial* serial, byte *array, unsigned int len);
void clearBuff();
void printBuff();

#endif //SOFTWARE_UTILS_H
