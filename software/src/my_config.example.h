#ifndef SOFTWARE_MY_CONFIG_H
#define SOFTWARE_MY_CONFIG_H

// uncomment if you want to change the remote config

// VITOTROL CONFIGURATION
//#define DEVICE_CLASS 0x11

#define MAX_SLOTS 3
#define ENABLE_SLOT_1
#define ENABLE_SLOT_2
//#define ENABLE_SLOT_3

// vitotrol 200
//#define DEVICE_ID 0x34
//#define DEVICE_SN1 0x00
//#define DEVICE_SN2 0x11
//#define DEVICE_SLOT 0x01

// if the DVICE_ID s 0x34 and 0x38 do not work, you can try all IDs by setting:
// I would first disable the remote on the heater, then flash the firmware and then enable the remote again
// this is a very slow process
//#define FIND_DEVICE_ID
//#define FIND_DEVICE_ID_START 0

// define wifi constants here, if portal is unwanted

#endif //SOFTWARE_MY_CONFIG_H
