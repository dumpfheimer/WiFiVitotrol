// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#ifndef WIFI_MGR_PORTAL_H
#define WIFI_MGR_PORTAL_H

#include <wifi_mgr.h>
#include <wifi_mgr_eeprom.h>

enum PortalConfigEntryType {
    STRING = 0,
    NUMBER = 1,
    BOOL = 2
};
struct PortalConfigEntry {
    PortalConfigEntryType type;
    const char* name;
    const char* eepromKey;
    bool isPassword;
    bool restartOnChange;
    PortalConfigEntry* next;
};

void wifiMgrPortalSetup(bool redirectIndex, const char* ssidPrefix, const char* password);
bool wifiMgrPortalLoop();
void wifiMgrPortalAddConfigEntry(const char* name, const char* eepromKey, PortalConfigEntryType type, bool isPassword, bool restartOnChange);


#endif //WIFI_MGR_PORTAL_H
