#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#if __has_include("my_config.h")
#include "my_config.h"
#endif

// optional, can be configured over hotspot
//#define WIFI_SSID "CHANGEME"
//#define WIFI_PASSWORD "CHANGEME"
//#define WIFI_HOST "Virtual Vitotrol"
#define REBOOT_TIMEOUT (1000*60*15)
#define MSG_LOG_SIZE 30

// if you want to use MQTT, uncomment these lines and replace them accordingly

// optional, can be configured over hotspot
//#define MQTT_HOST "CHANGEME"
//#define MQTT_USER "CHANGEME"
//#define MQTT_PASS "CHANGEME"

// VITOTROL CONFIGURATION
#ifndef DEVICE_CLASS
#define DEVICE_CLASS 0x11
#endif

// vitotrol 200
#ifndef DEVICE_ID
#define DEVICE_ID 0x34
#endif
#ifndef DEVICE_SN1
#define DEVICE_SN1 0x00
#endif
#ifndef DEVICE_SN2
#define DEVICE_SN2 0x05
#endif
#ifndef DEVICE_SLOT
#define DEVICE_SLOT 0x01
#endif

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
#define DEBUG false

#endif // CONFIGURATION_H
