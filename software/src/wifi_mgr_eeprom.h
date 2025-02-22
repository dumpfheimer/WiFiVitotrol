// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#ifndef WIFI_MGR_EEPROM_H
#define WIFI_MGR_EEPROM_H

#if __has_include("my_config.h")
#include "my_config.h"
#endif

#if __has_include("configuration.h")
#include "configuration.h"
#endif

#include <EEPROM.h>

bool wifiMgrCommitEEPROM();
void wifiMgrClearEEPROM();
const char* wifiMgrGetConfig(const char* name);
bool wifiMgrSetConfig(const char* name, const char* value);
long wifiMgrGetLongConfig(const char* name, long def);
bool wifiMgrSetLongConfig(const char* name, long val);
unsigned long wifiMgrGetUlongConfig(const char* name, unsigned long def);
bool wifiMgrSetUlongConfig(const char* name, unsigned long val);
bool wifiMgrGetBoolConfig(const char* name, bool def);
bool wifiMgrSetBoolConfig(const char* name, bool def);

#endif //WIFI_MGR_EEPROM_H
