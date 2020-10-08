#include "dataEnums.h"
#include "configuration.h"

//        SERIAL CONFIGURATION
#ifdef ESP8266
#include "SoftwareSerial.h"
#define OUT_SERIAL Serial1
#define IN_SERIAL Serial1
#define DEBUG_SERIAL Serial
#endif
#ifdef ESP32
#define OUT_SERIAL Serial2
#define IN_SERIAL Serial2
#define DEBUG_SERIAL Serial
#endif

//        VITOTROL CONFIGURATION
#define DEVICE_CLASS 0x11 // from https://github.com/boblegal31/Heater-remote/blob/1a857b10db96405937f65ed8338e314f57adaedf/NetRemote/example/inc/ViessMann.h
#define DEVICE_SLOT 0x01

#define DEVICE_ID 0x34
#define DEVICE_SN1 0x00
#define DEVICE_SN2 0x05


//        BUFFER CONFIGURATION
// stick to 255 to prevent uint8 of overflowing
#define BUFFER_LEN 255

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

void setup() {
  #ifdef ESP32
  OUT_SERIAL.begin(1200, SERIAL_8E2, 18, 19, false);
  #endif
  DEBUG_SERIAL.begin(500000);

  DEBUG_SERIAL.println("STARTING");

  clearBuff();

  clearResponseBuffer();
  configureData();

  setupWifi();
}

void loop() {
  // after timeout (1 hour) reboot.
  if (((lastHeaterCommandReceivedAt + 3600000) < millis() || (lastCommandReceivedAt + 3600000) < millis()) && millis() > 3600000) {
    DEBUG_SERIAL.println("REBOOT");
    DEBUG_SERIAL.flush();
    delay(1000);
    ESP.restart();
  }
  // only speak to heater when when no datapoint is preventing it
  if (!preventCommunication()) {
    if (millis() > bufferReadStart + 500 && bufferPointer > 0) {
      // receiving took longer than 1s. dump!
      DEBUG_SERIAL.println("\r\nTIMEOUT WHILE READING MESSAGE");
      DEBUG_SERIAL.println(expectedMessageLength);
      DEBUG_SERIAL.println(bufferPointer);
      DEBUG_SERIAL.println(bufferReadStart);
      printBuff();
      clearBuff();
    }
  
    while (IN_SERIAL.available()) {
      fakePingMessage = false;
      // if it is the first byte received, store the time (for timeout)
      if(bufferPointer == 0) bufferReadStart = millis();
      // read the byte
      byte receiveByte = IN_SERIAL.read();   // Read the byte

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
        DEBUG_SERIAL.println("BUFFER OVERFLOW");
        printBuff();
        clearBuff();
      }
      // if the message length matches the bufferPointer
      if (expectedMessageLength == bufferPointer && bufferPointer > 3) {
        // message received completely. work it!
        // first check the checksum
        if (isValidCRC(buffer, bufferPointer)) {
          // workMessageAndCreateResponseBuffer returnes true when the message was processed successfully
          if(workMessageAndCreateResponseBuffer(buffer, bufferPointer)) {
            // message was processed successfully. send the response
            sendResponse();
            
            if(IN_SERIAL.available()) {
              // somethimes the TI MBUS chip will read the values that are written. consume them to ensure the next bytes read are clean
              DEBUG_SERIAL.println("RECEIVED SOMETHING WHILE SENDING:");
              while(IN_SERIAL.available()) {
                //IN_SERIAL.read();
                DEBUG_SERIAL.print(IN_SERIAL.read(), HEX);
                DEBUG_SERIAL.print(" ");
              }
            }
    
            // debug print the request/response
            DEBUG_SERIAL.print("OK: ");
            // debug print the received message
            printAsHex(&DEBUG_SERIAL, buffer, bufferPointer);
            DEBUG_SERIAL.print("->");
            // debug print the response message
            printAsHex(&DEBUG_SERIAL, responseBuffer, responseBuffer[3]);
            DEBUG_SERIAL.println();
          } else {
            // debug print the unsuccessfully processed message
            DEBUG_SERIAL.print("NOK: ");
            printAsHex(&DEBUG_SERIAL, buffer, bufferPointer);
            DEBUG_SERIAL.println();
          }
        } else {
          // invalid message
          DEBUG_SERIAL.println("invalid message received:");
          printAsHex(&DEBUG_SERIAL, buffer, bufferPointer);
        }
        // clear buffer
        clearBuff();
      }
      yield();
    }
    
    // fake ping messages for testing
    if(fakePingMessage&& lastFakePing + 5000 < millis()) {
      lastFakePing = millis();
      DEBUG_SERIAL.println("FAKE PING");
      buffer[0] = 0x11;
      buffer[1] = 0x00;
      buffer[2] = 0x00;
      workMessageAndCreateResponseBuffer(buffer, 9);
      DEBUG_SERIAL.print("FAKE OK: ");
      printAsHex(&DEBUG_SERIAL, responseBuffer, responseBuffer[3]);
      DEBUG_SERIAL.println();
    }
  } else {
    // keep the serial buffer empty while not speaking to heater
    while(IN_SERIAL.available()) IN_SERIAL.read();
    if (bufferPointer > 0) clearBuff();

    // fake ping messages for testing
    if(fakePingMessage&& lastFakePing + 5000 < millis()) {
      lastFakePing = millis();
      DEBUG_SERIAL.println("FAKE PING: NOT CONNECTING");
    }
  }
  yield();
  // call wifi loop (and ultimately handle wifi clients)
  wifiLoop();
}
