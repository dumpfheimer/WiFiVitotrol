#include "main.h"

const long rebootTimeout = REBOOT_TIMEOUT; // 15 minutes

// the read buffer
// data we receive from the heater
byte buffer[BUFFER_LEN];
byte lastValidMessage[BUFFER_LEN];
byte lastInvalidMessage[BUFFER_LEN];
// the write buffer
byte responseBuffer[BUFFER_LEN];
// the next slot to write into the read buffer
uint8_t bufferPointer = 0;
// the time when we started to read the message
unsigned long bufferReadStart = 0;
// the length we are expecting
short expectedMessageLength = 0;
// time when the last command was received (wifi->esp)
unsigned long lastCommandReceivedAt = 0;
// time when the last character was received from heater
unsigned long lastReadAt = 0;
// time when the last message was received from heater
unsigned long lastMessageAt = 0;
// time when the last valid message was received from heater
unsigned long lastValidMessageAt = 0;
// time when the last command was received (heater->esp; other than discovery)
unsigned long lastHeaterCommandReceivedAt = 0;

unsigned long lastMessageWithResponseAt = 0;
unsigned long lastMessageWithoutResponseAt = 0;
unsigned long lastResponseWrittenAt = 0;
unsigned long lastBroadcastMessage = 0;
uint8_t requestDataset = 0;
uint8_t requestByte = 0;
int readInt = 0;
byte readByte = 0;

char* linkState = new char[LINK_STATE_LENGTH + 1];

#if defined(ESP8266)
SoftwareSerial softwareSerial = SoftwareSerial(MODBUS_RX, MODBUS_TX);
#endif
#if defined(FIND_DEVICE_ID)
bool deviceIdFound = false;
#endif

void serialLoop();

void setup() {
    strncpy(linkState, "Setup", LINK_STATE_LENGTH);
#if defined(ESP8266)
    ModbusSerial.begin(1200, MODBUS_BAUD);
#elif defined(ESP32)
    ModbusSerial.begin(1200, MODBUS_BAUD, MODBUS_RX, MODBUS_TX, false);
    ModbusSerial.setRxTimeout(1);
#endif


#if DEBUG == true
    DebugSerial.begin(500000);
    debugPrintln("STARTING");
#endif

    clearBuff(true);
    initRegisters();
#if defined(FIND_DEVICE_ID)
#if defined(FIND_DEVICE_ID_START)
    *getRegisterPointer(0xF9) = FIND_DEVICE_ID_START;
#else
    *getRegisterPointer(0xF8) = 0;
#endif
#endif

    clearResponseBuffer();
    configureData();

    setupWifi();
    mqttSetup();
}

unsigned long lastSerialLoop = 0;

void serialLoop() {
    if (!ModbusSerial.available() && (millis() - lastSerialLoop) < 1000) {
        // only run serialLoop if there is new data or last run was 1 sec ago
        return;
    }
    lastSerialLoop = millis();
    // only speak to heater when no datapoint is preventing it
    if (preventCommunication()) {
        bool wasAvail = false;
        while (ModbusSerial.available()) {
            ModbusSerial.read();
            wasAvail = true;
            lastReadAt = millis();
        }
        if (wasAvail) {
            strncpy(linkState, "Did not respond to heater because datapoint prevented it", LINK_STATE_LENGTH);
        }
        debugPrintln("not talking to heater");
        return;
    }

    if (millis() - lastReadAt > 100 && bufferPointer > 0) {
        // receiving took longer than 1s. dump!
        debugPrintln("\r\nTIMEOUT WHILE READING MESSAGE");
        debugPrintln(expectedMessageLength);
        debugPrintln(bufferPointer);
        debugPrintln(bufferReadStart);
        printBuff();
        clearBuff();
        bufferReadStart = 0;
    }

    // read while buffer is not full
    if (bufferPointer >= BUFFER_LEN) {
#if DEBUG == true
        debugPrintln("BUFFER OVERFLOW");
            printBuff();
#endif
        clearBuff();
    }
    // read from serial
    readInt = ModbusSerial.read();
    if (readInt >= 0) {
        // store as byte
        readByte = (byte) readInt;

        if (bufferPointer == 0) {
            // save time when first byte was read
            bufferReadStart = millis();
            // clear the buffer when first byte is read
            clearBuff(true);
        }

        // store byte in buffer
        buffer[bufferPointer++] = readByte;
        lastReadAt = millis();

        // the 4th byte contains the message length.
        // if the message is as long as the transmitted/expected length, process the message
        if (buffer[3] == bufferPointer && bufferPointer > 6) {
            // save time when last complete message was received
            lastMessageAt = millis();

            // check the checksum
            if (isValidCRC(buffer, bufferPointer)) {
                // save time when last valid, complete message was received
                lastValidMessageAt = millis();

                if (buffer[0] == 0xFF) {
                    // save time when last broadcast message was received
                    lastBroadcastMessage = millis();
                }
#ifdef FIND_DEVICE_ID
                if (buffer[0] == DEVICE_CLASS && buffer[4] == DEVICE_SLOT) {
                    if (buffer[2] != 0x00 && buffer[2] != 0x33) {
                        // device id seems to be good
                        deviceIdFound = true;
                    } else if (!deviceIdFound) {
                        uint8_t *did = getRegisterPointer(0xF9);
                        *did += 1;
                    }
                }
#endif

                if ((buffer[0] == 0xFF) || (buffer[0] == DEVICE_CLASS && buffer[4] == DEVICE_SLOT)) {
                    // a valid message was received (CRC match)
                    // copy invalid message to buffer of invalid message
                    memcpy(lastValidMessage, buffer, BUFFER_LEN);
                    if (workMessageAndCreateResponseBuffer(buffer)) {
                        // save time when a response is created for a message
                        lastMessageWithResponseAt = millis();
                        // message was processed successfully. send the response
                        sendResponse();
                        lastResponseWrittenAt = millis();

#if DEBUG == true
                        // debug print the request/response
                        debugPrint("OK: ");
                        // debug print the received message
                        printAsHex(&DebugSerial, buffer, bufferPointer);
                        debugPrint("->");
                        // debug print the response message
                        printAsHex(&DebugSerial, responseBuffer, responseBuffer[3]);
                        debugPrintln("");
#endif
                    } else {
                        strncpy(linkState,  "msg not successfully handled", LINK_STATE_LENGTH);
                        memcpy(lastInvalidMessage, buffer, BUFFER_LEN);
                        lastMessageWithoutResponseAt = millis();
#if DEBUG == true
                        // debug print the unsuccessfully processed message
                        debugPrint("NOK: ");
                        printAsHex(&DebugSerial, buffer, bufferPointer);
                        debugPrintln("");
#endif
                    }
                } else if (buffer[0] != 0x00 && buffer[1] != 0x11) {
                    snprintf(linkState, LINK_STATE_LENGTH, "received message for another device: %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3]);
                    // a message was received, that is not addressed to us
                    // copy message to buffer of invalid message
                    memcpy(lastInvalidMessage, buffer, BUFFER_LEN);
                }
                return;
            } else {
                strncpy(linkState, "message with invalid CRC received", LINK_STATE_LENGTH);
#if DEBUG == true
                // invalid message
                debugPrintln("invalid message received:");
                printAsHex(&DebugSerial, buffer, bufferPointer);
#endif
                // an invalid message was received (CRC mismatch)
                // copy invalid message to buffer of invalid message
                memcpy(lastInvalidMessage, buffer, BUFFER_LEN);
            }
            // clear buffer
            clearBuff();
        }
        yield();
    }
}

void loop() {
#ifndef WIFI_SSID
    if (wifiMgrPortalLoop()) {
#endif
        // after timeout (1 hour) reboot.
        if (((millis() - lastHeaterCommandReceivedAt > rebootTimeout) ||
             (millis() - lastCommandReceivedAt) > rebootTimeout)) {
            debugPrintln("REBOOT");
            delay(1000);
            ESP.restart();
        }

        mqttLoop();
        wifiLoop();
#ifndef WIFI_SSID
    }
#endif
    serialLoop();
}

void notifyCommandReceived() {
    lastCommandReceivedAt = millis();
}

const char* getLinkState() {
    linkState[LINK_STATE_LENGTH] = 0;
    return linkState;
}
void setLinkState(const char *newState) {
    strncpy(linkState, newState, LINK_STATE_LENGTH);
}
