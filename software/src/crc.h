//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_CRC_H
#define SOFTWARE_CRC_H

#include <Arduino.h>

bool isValidCRC(uint8_t data_p[], uint16_t length);

uint16_t calculateViessmannCRC(uint8_t data_p[], uint16_t length);

#endif //SOFTWARE_CRC_H
