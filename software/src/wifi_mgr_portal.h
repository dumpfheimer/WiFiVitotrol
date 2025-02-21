// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#ifndef WIFI_MGR_PORTAL_H
#define WIFI_MGR_PORTAL_H

#ifndef WIFI_MGR_PORTAL_PASSWORD
#define WIFI_MGR_PORTAL_PASSWORD "p0rtal123"
#endif

#include <wifi_mgr.h>
#include <wifi_mgr_eeprom.h>

enum PortalConfigEntryType {
    STRING = 0,
    NUMBER = 1
};
struct PortalConfigEntry {
    PortalConfigEntryType type;
    const char* name;
    const char* eepromKey;
    bool isPassword;
    bool restartOnChange;
    PortalConfigEntry* next;
};

void wifiMgrPortalSetup(bool redirectIndex);
bool wifiMgrPortalLoop();
void wifiMgrPortalAddConfigEntry(const char* name, const char* eepromKey, PortalConfigEntryType type, bool isPassword, bool restartOnChange);


#endif //WIFI_MGR_PORTAL_H
