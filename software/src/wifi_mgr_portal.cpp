// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#include "wifi_mgr_portal.h"

bool wifiMgrPortalIsSetup = false;
bool wifiMgrPortalStarted = false;
bool wifiMgrPortalConnectFailed = false;
bool wifiMgrPortalRedirectIndex = false;
bool wifiMgrPortalIsOwnServer = false;
XWebServer *wifiMgrPortalWebServer = nullptr;

PortalConfigEntry *firstEntry = nullptr;

PortalConfigEntry* getLastEntry() {
    PortalConfigEntry* tmp = firstEntry;
    if (tmp == nullptr) return nullptr;
    while (tmp->next != nullptr) tmp = tmp->next;
    return tmp;
}

void wifiMgrPortalSendConfigure() {
    PortalConfigEntry *tmp = firstEntry;
    int changes = 0;
    bool needRestart = false;
    bool isWifi = false;
    if (wifiMgrPortalWebServer->method() == HTTP_POST) {
        while (tmp != nullptr) {
            if (wifiMgrPortalWebServer->hasArg(tmp->eepromKey)) {
                String val = wifiMgrPortalWebServer->arg(tmp->eepromKey);
                const char *currentVal = wifiMgrGetConfig(tmp->eepromKey);
                // config item is in post
                if ((currentVal == nullptr || strcmp(val.c_str(), currentVal)) && (!tmp->isPassword || !wifiMgrPortalWebServer->arg(tmp->eepromKey).isEmpty())) {
                    // value changed
                    if (strcmp(tmp->eepromKey, "SSID") == 0 || strcmp(tmp->eepromKey, "PW") == 0 || strcmp(tmp->eepromKey, "HOST") == 0) {
                        isWifi = true;
                    }
                    if (tmp->restartOnChange) needRestart = true;
                    wifiMgrSetConfig(tmp->eepromKey, wifiMgrPortalWebServer->arg(tmp->eepromKey).c_str());
                    changes++;
                }
            }
            tmp = tmp->next;
        }
    }
    String ret = "<html><body><form action=# method=POST>";
    tmp = firstEntry;
    while (tmp != nullptr) {
        ret += "<h2>" + String(tmp->name) + "</h2><input type=\"text\" name=\"" + String(tmp->eepromKey) + "\"";
        if (!tmp->isPassword) {
            ret += " value=\"" + String(wifiMgrGetConfig(tmp->eepromKey)) + "\"";
        }
        ret += "><br/>";
        tmp = tmp->next;
    }
    ret +="<input type=submit></form>";
    if (changes > 0) ret += String(changes) + " changes made.";
    if (needRestart) ret += "will restart now.";
    if (wifiMgrPortalConnectFailed) ret += "last connect failed.";
    ret += "</html>";
    wifiMgrPortalWebServer->send(200, "text/html", ret);

    if (isWifi) {
        setupWifi(wifiMgrGetConfig("SSID"), wifiMgrGetConfig("WIFI_PW"));
        if (WiFi.isConnected()) {
            wifiMgrCommitEEPROM();
            wifiMgrPortalConnectFailed = false;
        } else {
            wifiMgrPortalIsSetup = false;
            wifiMgrPortalStarted = false;
            wifiMgrPortalConnectFailed = true;
            wifiMgrPortalLoop();
            return;
        }
    } else {
        wifiMgrCommitEEPROM();
    }
    if (needRestart) {
        delay(1000);
        ESP.restart();
    }
}

void wifiMgrPortalSetup(bool redirectIndex) {
    wifiMgrPortalRedirectIndex = redirectIndex;
    const char* ssid = wifiMgrGetConfig("SSID");
    wifiMgrPortalAddConfigEntry("SSID", "SSID", STRING, false, true);
    const char* pw = wifiMgrGetConfig("WIFI_PW");
    wifiMgrPortalAddConfigEntry("WiFi Password", "WIFI_PW", STRING, true, true);
    wifiMgrPortalAddConfigEntry("Hostname", "HOST", STRING, false, true);
    if (ssid != nullptr && pw != nullptr) {
        // configured
        const char* host = wifiMgrGetConfig("HOST");
        if (host == nullptr || strlen(host) == 0) {
#ifdef WIFI_MGR_HOSTNAME
            setupWifi(ssid, pw, WIFI_MGR_HOSTNAME);
#elifdef WIFI_MGR_HOSTNAME_PREFIX
            setupWifi(ssid, pw, WIFI_MGR_HOSTNAME_PREFIX + "-" + WiFi.macAddress());
#else
            setupWifi(ssid, pw);
#endif
        }
        else setupWifi(ssid, pw, host);
        wifiMgrPortalIsSetup = true;
    }
    wifiMgrPortalWebServer = wifiMgrGetWebServer();
    if (wifiMgrPortalWebServer == nullptr) {
        wifiMgrPortalWebServer = new XWebServer(80);
        wifiMgrPortalIsOwnServer = true;
    }
    wifiMgrPortalWebServer->begin();
    wifiMgrPortalWebServer->on("/wifiMgr/configure", HTTP_POST, wifiMgrPortalSendConfigure);
    wifiMgrPortalWebServer->on("/wifiMgr/configure", HTTP_GET, wifiMgrPortalSendConfigure);
    if (wifiMgrPortalRedirectIndex) {
        // TODO: actually do a redirect
        wifiMgrPortalWebServer->on("/", HTTP_POST, wifiMgrPortalSendConfigure);
        wifiMgrPortalWebServer->on("/", HTTP_GET, wifiMgrPortalSendConfigure);
    }
}

void wifiMgrPortalAddConfigEntry(const char* name, const char* eepromKey, PortalConfigEntryType type, bool isPassword, bool restartOnChange) {
    PortalConfigEntry *newEntry = new PortalConfigEntry();
    newEntry->name = name;
    newEntry->eepromKey = eepromKey;
    newEntry->type = type;
    newEntry->isPassword = isPassword;
    newEntry->restartOnChange = restartOnChange;

    PortalConfigEntry* last = getLastEntry();
    if (last == nullptr) firstEntry = newEntry;
    else last->next = newEntry;
}

bool wifiMgrPortalLoop() {
    if (wifiMgrPortalIsSetup) {
        loopWifi();
        return true;
    } else if (!wifiMgrPortalStarted) {
        String macAddress = WiFi.macAddress();
        macAddress.replace(":", "");
        WiFi.softAP("Portal-" + macAddress, WIFI_MGR_PORTAL_PASSWORD);

        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->begin();

        wifiMgrPortalStarted = true;
    } else {
        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->handleClient();
    }
    return false;
}