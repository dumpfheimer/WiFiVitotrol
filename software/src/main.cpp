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
// time when the last command was received (heater->esp; other than discovery)
unsigned long lastHeaterCommandReceivedAt = 0;
// fake a ping message for testing purposes
bool fakePingMessage = false;
unsigned long lastFakePing = 0;

//SoftwareSerial ModbusSerial(18, 19, false);

#if THREADED == true
TaskHandle_t serialTaskHandle;
#endif

void setup() {
    ModbusSerial.begin(1200, MODBUS_BAUD, 18, 19, false);
    DebugSerial.begin(9600);
    DebugSerial.println("STARTING");

    clearBuff();

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
                      &serialTaskHandle,      /* Task handle to keep track of created task */
                      1);          /* pin task to core 1 */
#endif
}

void serialLoop() {
    // after timeout (1 hour) reboot.
    if (((millis() - lastHeaterCommandReceivedAt > rebootTimeout) ||
            (millis() - lastCommandReceivedAt) > rebootTimeout)) {
        DebugSerial.println("REBOOT");
        DebugSerial.flush();
        delay(1000);
        ESP.restart();
    }
    // only speak to heater when no datapoint is preventing it
    if (!preventCommunication()) {
        if (millis() > bufferReadStart + 500 && bufferPointer > 0) {
            // receiving took longer than 1s. dump!
            DebugSerial.println("\r\nTIMEOUT WHILE READING MESSAGE");
            DebugSerial.println(expectedMessageLength);
            DebugSerial.println(bufferPointer);
            DebugSerial.println(bufferReadStart);
            printBuff();
            clearBuff();
        }

        while (ModbusSerial.available()) {
            fakePingMessage = false;
            // if it is the first byte received, store the time (for timeout)
            if (bufferPointer == 0) bufferReadStart = millis();
            // read the byte
            byte receiveByte = ModbusSerial.read();   // Read the byte

            // read while buffer is not full
            if (bufferPointer < BUFFER_LEN) {
                // still receiving message. fill buffer
                buffer[bufferPointer] = receiveByte;
                // the length of the message is sent at pos 3. store it as expectedMessageLength for readability
                if (bufferPointer == 3) {
                    expectedMessageLength = receiveByte;
                }
                bufferPointer++;
            } else {
                DebugSerial.println("BUFFER OVERFLOW");
                printBuff();
                clearBuff();
            }
            // if the message length matches the bufferPointer
            if (expectedMessageLength == bufferPointer && bufferPointer > 3) {
                // message received completely. work it!
                // first check the checksum
                if (isValidCRC(buffer, bufferPointer)) {
                    // workMessageAndCreateResponseBuffer returnes true when the message was processed successfully
                    if (workMessageAndCreateResponseBuffer(buffer, bufferPointer)) {
                        // message was processed successfully. send the response
                        sendResponse();

                        if (ModbusSerial.available()) {
                            // somethimes the TI MBUS chip will read the values that are written. consume them to ensure the next bytes read are clean
                            DebugSerial.println("RECEIVED SOMETHING WHILE SENDING:");
                            while (ModbusSerial.available()) {
                                //ModbusSerial.read();
                                DebugSerial.print(ModbusSerial.read(), HEX);
                                DebugSerial.print(" ");
                            }
                        }

                        // debug print the request/response
                        DebugSerial.print("OK: ");
                        // debug print the received message
                        printAsHex(&DebugSerial, buffer, bufferPointer);
                        DebugSerial.print("->");
                        // debug print the response message
                        printAsHex(&DebugSerial, responseBuffer, responseBuffer[3]);
                        DebugSerial.println();
                    } else {
                        // debug print the unsuccessfully processed message
                        DebugSerial.print("NOK: ");
                        printAsHex(&DebugSerial, buffer, bufferPointer);
                        DebugSerial.println();
                    }
                } else {
                    // invalid message
                    DebugSerial.println("invalid message received:");
                    printAsHex(&DebugSerial, buffer, bufferPointer);
                }
                // clear buffer
                clearBuff();
            }
            yield();
        }

        // fake ping messages for testing
        if (fakePingMessage && lastFakePing + 5000 < millis()) {
            lastFakePing = millis();
            DebugSerial.println("FAKE PING");
            buffer[0] = 0x11;
            buffer[1] = 0x00;
            buffer[2] = 0x00;
            workMessageAndCreateResponseBuffer(buffer, 9);
            DebugSerial.print("FAKE OK: ");
            printAsHex(&DebugSerial, responseBuffer, responseBuffer[3]);
            DebugSerial.println();
        }
    } else {
        // keep the serial buffer empty while not speaking to heater
        while (ModbusSerial.available()) ModbusSerial.read();
        if (bufferPointer > 0) clearBuff();

        // fake ping messages for testing
        if (fakePingMessage && lastFakePing + 5000 < millis()) {
            lastFakePing = millis();
            DebugSerial.println("FAKE PING: NOT CONNECTING");
        }
    }
    yield();
}

void serialLoopForever(void *pvParameters) {
    while (1) serialLoop();
}

void loop() {
#if THREADED == false
    serialLoop();
#endif
    // call wifi loop (and ultimately handle wifi clients)
    wifiLoop();
}
