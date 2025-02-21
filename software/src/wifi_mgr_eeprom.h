// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#ifndef WIFI_MGR_EEPROM_H
#define WIFI_MGR_EEPROM_H

#include <EEPROM.h>

void wifiMgrCommitEEPROM();
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
