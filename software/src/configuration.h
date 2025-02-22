#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define WIFI_MGR_EEPROM_START_ADDR 1
// optional, can be configured over hotspot
//#define WIFI_SSID "CHANGEME"
//#define WIFI_PASSWORD "CHANGEME"
//#define WIFI_HOST "Virtual Vitotrol"
#define REBOOT_TIMEOUT (1000*60*15)



// if you want to use MQTT, uncomment these lines and replace them accordingly

// optional, can be configured over hotspot
//#define MQTT_HOST "CHANGEME"
//#define MQTT_USER "CHANGEME"
//#define MQTT_PASS "CHANGEME"

// VITOTROL CONFIGURATION
#define DEVICE_CLASS 0x11

// vitotrol 200
#define DEVICE_ID 0x34
#define DEVICE_SN1 0x00
#define DEVICE_SN2 0x05
#define DEVICE_SLOT 0x01

// vitotrol 300
//#define DEVICE_ID 0x38
//#define DEVICE_SN1 0x00
//#define DEVICE_SN2 0x11
//#define DEVICE_SLOT 0x02

#if defined(ESP32)
#define MODBUS_BAUD SERIAL_8E1
#define ModbusSerial Serial1
#define DebugSerial Serial
#define MODBUS_RX 17
#define MODBUS_TX 16
#define WIFI_MGR_EEPROM_SIZE 4096
#elif defined(ESP8266)
#include <SoftwareSerial.h>
extern SoftwareSerial softwareSerial;
#define MODBUS_BAUD SWSERIAL_8E1
#define ModbusSerial softwareSerial
#define DebugSerial Serial
#define MODBUS_RX D4
#define MODBUS_TX D5
#define WIFI_MGR_EEPROM_SIZE 1024
#endif
#define DEBUG true

#endif // CONFIGURATION_H
