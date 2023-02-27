//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_UTILS_H
#define SOFTWARE_UTILS_H

#include <Arduino.h>

#include "main.h"
#if DEBUG == false
#define debugPrintln(x) (void)0
#define debugPrint(x) (void)0
#else
#define debugPrintln(x) DebugSerial.println(x)
#define debugPrint(x) DebugSerial.print(x)
#endif
void printAsHex(Stream *serial, byte *array, unsigned int len);
void clearBuff();
void clearBuff(bool zero);
void printBuff();
void debugPrintHex(byte x);

#endif //SOFTWARE_UTILS_H
