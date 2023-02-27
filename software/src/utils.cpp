#include "utils.h"

void printAsHex(Stream *serial, byte *array, unsigned int len) {
    serial->print("0x");
    for (unsigned int i = 0; i < len; i++) {
        if (array[i] >= 16) {
            serial->print(array[i], HEX);
        } else {
            serial->print('0');
            serial->print(array[i], HEX);
        }
    }
}
void clearBuff(bool zero) {
    if (zero) {
        for (int i = 0; i < BUFFER_LEN; i++) buffer[i] = 0;
    }
    bufferPointer = 0;
}
void clearBuff() {
    clearBuff(false);
}

void printBuff() {
#if DEBUG == true
    uint8_t i = 0;
    debugPrintln("BUFFER:");
    for (i = 0; i < bufferPointer; i = i + 1) {
        debugPrintHex(buffer[i]);
        debugPrint(" ");
    }
    debugPrintln("");
#endif
}
void debugPrintHex(byte x) {
#if DEBUG == true
    DebugSerial.print(x, HEX);
#endif
}