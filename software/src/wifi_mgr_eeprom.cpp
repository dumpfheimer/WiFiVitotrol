// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#include "wifi_mgr_eeprom.h"

//  EEPROM SETUP
// HEADER HEADER VERSION VERSION NR_ENTRIES
// FOR EACH ENTRY:
// LENGTH_OF_NAME LENGTH_OF VALUE NAME NAME NAME NAME VALUE VALUE VALUE

#define WIFI_MGR_MAX_CONFIG_ENTRIES 32

#define WIFI_MGR_EEPROM_HEADER_1 0x43
#define WIFI_MGR_EEPROM_HEADER_2 0x96
#define WIFI_MGR_EEPROM_VERSION_1 0x00
#define WIFI_MGR_EEPROM_VERSION_2 0x01

bool initialized = false;

struct CacheEntry {
    char* name = nullptr;
    size_t nameLen = 0;
    char* value = nullptr;
    size_t valueLen = 0;
};
CacheEntry* cache = new CacheEntry[WIFI_MGR_MAX_CONFIG_ENTRIES];

CacheEntry* nextEmptyCacheEntry() {
    for (int i = 0; i < WIFI_MGR_MAX_CONFIG_ENTRIES; i++) {
        if (cache[i].name == nullptr) return &cache[i];
    }
    return nullptr;
}

void wifiMgrSetupEEPROM() {
    if (initialized) return;
#ifdef WIFI_MGR_EEPROM_SIZE
    EEPROM.begin(WIFI_MGR_EEPROM_SIZE);
#else
    EEPROM.begin(EEPROM.length());
#endif

    //EEPROM.r
    uint8_t header1 = EEPROM.read(WIFI_MGR_EEPROM_START_ADDR + 0);
    uint8_t header2 = EEPROM.read(WIFI_MGR_EEPROM_START_ADDR + 1);
    uint8_t version1 = EEPROM.read(WIFI_MGR_EEPROM_START_ADDR + 2);
    uint8_t version2 = EEPROM.read(WIFI_MGR_EEPROM_START_ADDR + 3);
    uint8_t numberOfEntries = EEPROM.read(WIFI_MGR_EEPROM_START_ADDR + 4);

    if (header1 != 0x43 || header2 != 0x96 || version1 != 0x00 || version2 != 0x01) {
        // not initialized
        EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 0, WIFI_MGR_EEPROM_HEADER_1);
        EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 1, WIFI_MGR_EEPROM_HEADER_2);
        EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 2, WIFI_MGR_EEPROM_VERSION_1);
        EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 3, WIFI_MGR_EEPROM_VERSION_2);
        EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 4, 0x00);
        numberOfEntries = 0;
    }

    uint16_t eepromPtr = WIFI_MGR_EEPROM_START_ADDR + 5;
    for (int i = 0; i < numberOfEntries; i++) {
        uint8_t entryNameLength = EEPROM.read(eepromPtr);
        uint8_t entryValueLength = EEPROM.read(eepromPtr + 1 + entryNameLength);

        if (entryNameLength != 0 && entryValueLength != 0) {
            auto *newEntry = nextEmptyCacheEntry();
            newEntry->name = new char[entryNameLength + 1];
            newEntry->name[entryNameLength] = 0;
            newEntry->nameLen = entryNameLength;
            newEntry->value = new char[entryValueLength + 1];
            newEntry->value[entryValueLength] = 0;
            newEntry->valueLen = entryValueLength;

            for (int ri = 0; ri < entryNameLength && ri < 256; ri++) {
                newEntry->name[ri] = (char) EEPROM.read(eepromPtr + 1 + ri);
            }
            for (int ri = 0; ri < entryValueLength && ri < 256; ri++) {
                newEntry->value[ri] = (char) EEPROM.read(eepromPtr + 2 + entryNameLength + ri);
            }
        }
        eepromPtr += 2 + entryNameLength + entryValueLength;
    }
    initialized = true;
}
CacheEntry* getCacheEntryByName(const char* name) {
    wifiMgrSetupEEPROM();
    for (int i = 0; i < WIFI_MGR_MAX_CONFIG_ENTRIES; i++) {
        if (cache[i].name != nullptr && strcmp(name, cache[i].name) == 0) {
            return &cache[i];
        }
    }
    return nullptr;
}
bool wifiMgrCommitEEPROM() {
    wifiMgrSetupEEPROM();
    EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 0, WIFI_MGR_EEPROM_HEADER_1);
    EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 1, WIFI_MGR_EEPROM_HEADER_2);
    EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 2, WIFI_MGR_EEPROM_VERSION_1);
    EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 3, WIFI_MGR_EEPROM_VERSION_2);
    uint16_t eepromPtr = WIFI_MGR_EEPROM_START_ADDR + 5;
    uint8_t count = 0;
    for (int i = 0; i < WIFI_MGR_MAX_CONFIG_ENTRIES; i++) {
        CacheEntry cacheEntry = cache[i];
        if (cacheEntry.value != nullptr && cacheEntry.name != nullptr) {
            EEPROM.write(eepromPtr++, cacheEntry.nameLen);
            for (unsigned int wi = 0; wi < cacheEntry.nameLen; wi++) {
                EEPROM.write(eepromPtr++, cacheEntry.name[wi]);
            }
            EEPROM.write(eepromPtr++, cacheEntry.valueLen);
            for (unsigned int wi = 0; wi < cacheEntry.valueLen; wi++) {
                EEPROM.write(eepromPtr++, cacheEntry.value[wi]);
            }
            count++;
        }
    }
    EEPROM.write(WIFI_MGR_EEPROM_START_ADDR + 4, count);
    return EEPROM.commit();
}
void wifiMgrClearEEPROM() {
    wifiMgrSetupEEPROM();
    for (int i = 0; i < WIFI_MGR_MAX_CONFIG_ENTRIES; i++) {
        CacheEntry cacheEntry = cache[i];
        if (cacheEntry.name != nullptr) delete[] cacheEntry.name;
        cacheEntry.nameLen = 0;
        if (cacheEntry.value != nullptr) delete[] cacheEntry.value;
        cacheEntry.valueLen = 0;
    }
}
const char* wifiMgrGetConfig(const char* name) {
    wifiMgrSetupEEPROM();
    CacheEntry* cacheEntry = getCacheEntryByName(name);
    if (cacheEntry == nullptr) return nullptr;
    else return cacheEntry->value;
}
bool wifiMgrSetConfig(const char* name, const char* value) {
    wifiMgrSetupEEPROM();
    CacheEntry* cacheEntry = getCacheEntryByName(name);

    if (cacheEntry == nullptr) {
        cacheEntry = nextEmptyCacheEntry();
        if (cacheEntry == nullptr) return false;

        size_t len = strlen(name);
        cacheEntry->name = new char[len + 1];
        if (cacheEntry->name == nullptr) return false;
        strcpy(cacheEntry->name, name);
        cacheEntry->nameLen = len;
    }

    if (cacheEntry->value != nullptr) delete[] cacheEntry->value;
    size_t len = strlen(value);
    cacheEntry->value = new char[len + 1];
    if (cacheEntry->value == nullptr) return false;
    strcpy(cacheEntry->value, value);
    cacheEntry->valueLen = len;
    return true;
}
bool wifiMgrSetConfig(const char* name, const char* value, uint8_t len) {
    wifiMgrSetupEEPROM();
    CacheEntry* cacheEntry = getCacheEntryByName(name);

    if (cacheEntry == nullptr) {
        cacheEntry = nextEmptyCacheEntry();
        if (cacheEntry == nullptr) return false;

        size_t nameLen = strlen(name);
        cacheEntry->name = new char[nameLen + 1];
        if (cacheEntry->name == nullptr) return false;
        strcpy(cacheEntry->name, name);
        cacheEntry->nameLen = nameLen;
    }

    if (cacheEntry->value != nullptr) delete[] cacheEntry->value;
    cacheEntry->value = new char[len];
    if (cacheEntry->value == nullptr) return false;
    for (uint16_t wi = 0; wi < len; wi++) {
        cacheEntry->value[wi] = (char) value[wi];
    }
    cacheEntry->valueLen = len;
    return true;
}
long wifiMgrGetLongConfig(const char* name, long def) {
    long load;
    CacheEntry* cacheEntry = getCacheEntryByName(name);
    if (cacheEntry == nullptr || cacheEntry->valueLen != 4) {
        return def;
    }
    load = 0;
    load |= cacheEntry->value[0] << 24;
    load |= cacheEntry->value[1] << 16;
    load |= cacheEntry->value[2] << 8;
    load |= cacheEntry->value[3];
    return load;
}
bool wifiMgrSetLongConfig(const char* name, long val) {
    char tmp[] = {0,0,0,0};
    tmp[0] = val >> 24 & 0xFF;
    tmp[1] = val >> 16 & 0xFF;
    tmp[2] = val >> 8 & 0xFF;
    tmp[3] = val & 0xFF;
    return wifiMgrSetConfig(name, tmp, 4);
}
unsigned long wifiMgrGetUlongConfig(const char* name, unsigned long def) {
    unsigned long load;
    CacheEntry* cacheEntry = getCacheEntryByName(name);
    if (cacheEntry == nullptr || cacheEntry->valueLen != 4) {
        return def;
    }
    load = 0;
    load |= cacheEntry->value[0] << 24;
    load |= cacheEntry->value[1] << 16;
    load |= cacheEntry->value[2] << 8;
    load |= cacheEntry->value[3];
    return load;
}
bool wifiMgrSetUlongConfig(const char* name, unsigned long val) {
    char tmp[] = {0,0,0,0};
    tmp[0] = val >> 24 & 0xFF;
    tmp[1] = val >> 16 & 0xFF;
    tmp[2] = val >> 8 & 0xFF;
    tmp[3] = val & 0xFF;
    return wifiMgrSetConfig(name, tmp, 4);
}
bool wifiMgrGetBoolConfig(const char* name, bool def) {
    CacheEntry* cacheEntry = getCacheEntryByName(name);
    if (cacheEntry == nullptr || cacheEntry->valueLen != 1) {
        return def;
    }
    return cacheEntry->value[0] != 0;
}
bool wifiMgrSetBoolConfig(const char* name, bool val) {
    char tmp[1] = {0};
    if (val) tmp[0] = 1;
    return wifiMgrSetConfig(name, tmp, 1);
}
