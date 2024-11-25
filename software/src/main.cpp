#include "main.h"

const char *wifiSSID = WIFI_SSID;
const char *wifiPassword = WIFI_PASSWORD;
const char *wifiHost = WIFI_HOST;
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
int readInt = 0;
byte readByte = 0;

void serialLoop();

void setup() {
    ModbusSerial.begin(1200, MODBUS_BAUD, 17, 16, false);

    ModbusSerial.onReceive(serialLoop, false);
    ModbusSerial.setRxTimeout(1);

#if DEBUG == true
    DebugSerial.begin(500000);
    debugPrintln("STARTING");
#endif

    clearBuff(true);
    initRegisters();

    clearResponseBuffer();
    configureData();

    setupWifi();
#ifdef MQTT_HOST
    mqttSetup();
#endif
}

void serialLoop() {
    // only speak to heater when no datapoint is preventing it
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

                if ((buffer[0] == 0xFF) || (buffer[0] == DEVICE_CLASS && buffer[4] == DEVICE_SLOT)) {
                    // a valid message was received (CRC match)
                    // copy invalid message to buffer of invalid message
                    memcpy(lastValidMessage, buffer, BUFFER_LEN);
                    if (workMessageAndCreateResponseBuffer(buffer, bufferPointer)) {
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
                        lastMessageWithoutResponseAt = millis();
#if DEBUG == true
                        // debug print the unsuccessfully processed message
                        debugPrint("NOK: ");
                        printAsHex(&DebugSerial, buffer, bufferPointer);
                        debugPrintln("");
#endif
                    }
                } else {
                    // a message was received, that is not addressed to us
                    // copy message to buffer of invalid message
                    memcpy(lastInvalidMessage, buffer, BUFFER_LEN);
                }
                return;
            } else {
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
    // after timeout (1 hour) reboot.
    if (((millis() - lastHeaterCommandReceivedAt > rebootTimeout) ||
         (millis() - lastCommandReceivedAt) > rebootTimeout)) {
        debugPrintln("REBOOT");
        delay(1000);
        ESP.restart();
    }

    // call wifi loop (and ultimately handle wifi clients)
    wifiLoop();
#ifdef MQTT_HOST
    mqttLoop();
#endif
}

void notifyCommandReceived() {
    lastCommandReceivedAt = millis();
}
