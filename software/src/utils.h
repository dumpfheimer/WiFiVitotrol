//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_UTILS_H
#define SOFTWARE_UTILS_H

#include <Arduino.h>

#include "main.h"

void printAsHex(Stream *serial, byte *array, unsigned int len);

void clearBuff();

void printBuff();

#endif //SOFTWARE_UTILS_H
