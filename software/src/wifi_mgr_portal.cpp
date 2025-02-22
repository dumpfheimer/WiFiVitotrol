// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#include "wifi_mgr_portal.h"

bool wifiMgrPortalIsSetup = false;
bool wifiMgrPortalStarted = false;
bool wifiMgrPortalConnectFailed = false;
bool wifiMgrPortalCommitFailed = false;
bool wifiMgrPortalRedirectIndex = false;
bool wifiMgrPortalIsOwnServer = false;
XWebServer *wifiMgrPortalWebServer = nullptr;
const char *ssidPrefix = nullptr;
const char *password = nullptr;

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
            if (wifiMgrPortalWebServer->hasArg(tmp->eepromKey) && !wifiMgrPortalWebServer->arg(tmp->eepromKey).isEmpty()) {
                String val = wifiMgrPortalWebServer->arg(tmp->eepromKey);
                const char *currentVal = wifiMgrGetConfig(tmp->eepromKey);
                // config item is in post
                if ((currentVal == nullptr || strcmp(val.c_str(), currentVal)) && (!tmp->isPassword || !wifiMgrPortalWebServer->arg(tmp->eepromKey).isEmpty())) {
                    // value changed
                    if (strcmp(tmp->eepromKey, "SSID") == 0 || strcmp(tmp->eepromKey, "PW") == 0 || strcmp(tmp->eepromKey, "HOST") == 0) {
                        isWifi = true;
                    }
                    if (tmp->restartOnChange) needRestart = true;
                    if (tmp->type == STRING) {
                        wifiMgrSetConfig(tmp->eepromKey, wifiMgrPortalWebServer->arg(tmp->eepromKey).c_str());
                    } else if (tmp->type == NUMBER) {
                        wifiMgrSetLongConfig(tmp->eepromKey, wifiMgrPortalWebServer->arg(tmp->eepromKey).toInt());
                    } else if (tmp->type == BOOL) {
                        wifiMgrSetBoolConfig(tmp->eepromKey, wifiMgrPortalWebServer->arg(tmp->eepromKey) == "1");
                    }
                    changes++;
                }
            }
            tmp = tmp->next;
        }
        //wifiMgrPortalWebServer->sendHeader("Location", "/wifiMgr/configure");
        //wifiMgrPortalWebServer->send(301);
    }
    String ret = "<html><body><form action=# method=POST>";
    tmp = firstEntry;
    while (tmp != nullptr) {
        ret += "<h2>" + String(tmp->name) + "</h2>";
        if (tmp->type == STRING) {
            ret += "<input type=\"text\" name=\"" + String(tmp->eepromKey) + "\"";
            if (!tmp->isPassword) {
                ret += " value=\"" + String(wifiMgrGetConfig(tmp->eepromKey)) + "\"";
            }
        } else if (tmp->type == NUMBER) {
            ret += "<input type=\"number\" name=\"" + String(tmp->eepromKey) + "\"";
            if (!tmp->isPassword) {
                ret += " value=\"" + String(wifiMgrGetConfig(tmp->eepromKey)) + "\"";
            }
        } else if (tmp->type == BOOL) {
            ret += "<select name=\"" + String(tmp->eepromKey) + "\">";
            ret += "<option value=\"1\"";
            if (wifiMgrGetBoolConfig(tmp->eepromKey, false)) ret += " selected";
            ret += ">yes / on</option>";
            ret += "<option value=\"0\"";
            if (wifiMgrGetBoolConfig(tmp->eepromKey, true) == false) ret += " selected";
            ret += ">no / off</option>";
            ret += "</select>";
        }
        ret += "<br/>";
        tmp = tmp->next;
    }
    ret +="<input type=submit></form>";
    if (changes > 0) ret += String(changes) + " changes made.";
    if (needRestart) ret += "will restart now.";
    if (wifiMgrPortalConnectFailed) ret += "last connect failed.";

    if (wifiMgrPortalWebServer->method() == HTTP_POST) {
        if (isWifi) {
            ret += "Testing WiFi</body></html>";
            wifiMgrPortalWebServer->send(200, "text/html", ret);
            delay(500);
            setupWifi(wifiMgrGetConfig("SSID"), wifiMgrGetConfig("WIFI_PW"));
            if (WiFi.isConnected()) {
                if (!wifiMgrCommitEEPROM()) {
                    wifiMgrPortalCommitFailed = false;
                }
                wifiMgrPortalConnectFailed = false;
            } else {
                wifiMgrPortalIsSetup = false;
                wifiMgrPortalStarted = false;
                wifiMgrPortalConnectFailed = true;
                wifiMgrPortalLoop();
                return;
            }
        } else {
            if (!wifiMgrCommitEEPROM()) {
                ret += "EEPROM write failed</body></html>";
                wifiMgrPortalWebServer->send(200, "text/html", ret);
                wifiMgrPortalCommitFailed = false;
            } else {
                ret += "settings saved</body></html>";
                wifiMgrPortalWebServer->send(200, "text/html", ret);
            }
        }
        if (needRestart) {
            delay(1000);
            ESP.restart();
        }
    } else {
        ret += "</body></html>";
        wifiMgrPortalWebServer->send(200, "text/html", ret);
    }
}

void wifiMgrPortalSetup(bool redirectIndex, const char* ssidPrefix_, const char* password_) {
    ssidPrefix = ssidPrefix_;
    password = password_;
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
            String macAddress = WiFi.macAddress();
            macAddress.replace(":", "");
            macAddress = macAddress.substring(6, macAddress.length());

            setupWifi(ssid, pw, (ssidPrefix + macAddress).c_str());
#endif
        }
        else setupWifi(ssid, pw, host);

        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->begin();
        wifiMgrPortalIsSetup = true;
    }
    wifiMgrPortalWebServer = wifiMgrGetWebServer();
    if (wifiMgrPortalWebServer == nullptr) {
        wifiMgrPortalWebServer = new XWebServer(80);
        wifiMgrPortalIsOwnServer = true;
    }
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
        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->handleClient();
        return true;
    } else if (!wifiMgrPortalStarted) {
        String macAddress = WiFi.macAddress();
        macAddress.replace(":", "");
        macAddress = macAddress.substring(6, macAddress.length());
        WiFi.softAP(ssidPrefix + macAddress, password);

        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->begin();

        wifiMgrPortalStarted = true;
    } else {
        if (wifiMgrPortalWebServer != nullptr) wifiMgrPortalWebServer->handleClient();
    }
    return false;
}