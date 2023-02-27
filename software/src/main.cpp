#include "main.h"

const char *wifiSSID = WIFI_SSID;
const char *wifiPassword = WIFI_PASSWORD;
const char *wifiHost = WIFI_HOST;
const long rebootTimeout = REBOOT_TIMEOUT; // 15 minutes

// the read buffer
// data we receive from the heater
byte buffer[BUFFER_LEN];
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
unsigned long lastResponseTime = 0;
unsigned long sendDelay = 0;
byte readByte = 0;

#if THREADED == true
TaskHandle_t serialTaskHandle;
#endif

void serialLoop();

void setup() {
    ModbusSerial.begin(1200, MODBUS_BAUD, 17, 16, false);
#if THREADED == false
    ModbusSerial.onReceive(serialLoop, false);
    ModbusSerial.setRxTimeout(1);
#endif
#if DEBUG == true
    DebugSerial.begin(500000);
    debugPrintln("STARTING");
#endif

    clearBuff();
    initRegisters();

    clearResponseBuffer();
    configureData();

    setupWifi();

#if THREADED == true
    xTaskCreatePinnedToCore(
                      serialLoopForever,   /* Task function. */
                      "serialLoopTask",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &serialTaskHandle      /* Task handle to keep track of created task */
                      , 1);          /* pin task to core 1 */
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

    while (ModbusSerial.available()) {
        // if it is the first byte received, store the time (for timeout)
        if (bufferPointer == 0) {
            bufferReadStart = millis();
        }
        // read the byte
        readByte = ModbusSerial.read();
        if (bufferPointer == 0 && readByte != 0x11) {
            return;
        }
        if (bufferPointer == 0) {
            clearBuff(true);
        }

        buffer[bufferPointer++] = readByte;   // Read the byte
        lastReadAt = millis();

        // if the message length matches the bufferPointer
        if (buffer[3] == bufferPointer && bufferPointer > 4) {
            lastMessageAt = millis();
            // message received completely. work it!
            // first check the checksum
            if (isValidCRC(buffer, bufferPointer)) {
                lastValidMessageAt = millis();
                // workMessageAndCreateResponseBuffer returnes true when the message was processed successfully
                if (workMessageAndCreateResponseBuffer(buffer, bufferPointer)) {
                    lastMessageWithResponseAt = millis();
                    delay(sendDelay);
                    // message was processed successfully. send the response
                    sendResponse();
                    lastResponseWrittenAt = millis();
                    lastResponseTime = lastMessageWithResponseAt - lastReadAt;

                    delay(100);
                    while (ModbusSerial.available()) ModbusSerial.read();
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
                return;
            } else {
#if DEBUG == true
                // invalid message
                debugPrintln("invalid message received:");
                printAsHex(&DebugSerial, buffer, bufferPointer);
#endif
            }
            // clear buffer
            clearBuff();
        }
        yield();
    }
}

void serialLoopForever(void *pvParameters) {
    while (1) {
        serialLoop();
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
}
