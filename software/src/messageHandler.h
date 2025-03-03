//
// Created by chris on 21.02.23.
//

#ifndef SOFTWARE_MESSAGEHANDLER_H
#define SOFTWARE_MESSAGEHANDLER_H

#define MSG_PING 0x00
#define MSG_REQUEST_1_BYTE 0x31
#define MSG_REQUEST_N_BYTES 0x33
// 11-0-3f-12-1-1-34-a8-a8-a2-4b-ae-65-fd-16-cd-56-be-
#define MSG_UNKNOWN 0x3F
#define MSG_PONG 0x80
#define MSG_SENDING_1_BYTE 0xB1
#define MSG_SENDING_N_BYTES 0xB3
#define MSG_SENDING_COMMAND 0xBF

#include "writableData.h"
#include "responseHelper.h"
#include "registers.h"
#include "dataset.h"

bool workMessageAndCreateResponseBuffer(byte buff[]);

#endif //SOFTWARE_MESSAGEHANDLER_H
