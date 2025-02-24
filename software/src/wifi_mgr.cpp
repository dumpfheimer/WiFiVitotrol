// PUBLISHED UNDER CC BY-NC 3.0 https://creativecommons.org/licenses/by-nc/3.0/

#include "wifi_mgr.h"

#if defined(ESP8266)
MDNSResponder wifiMgrMdns;
#elif defined(ESP32)
#endif

unsigned long wifiMgrLastNonShitRSS = 0;
unsigned long wifiMgrlastConnected = 0;
unsigned long wifiMgrNotifyNoWifiTimeout = 600 * 1000; // 10m
unsigned long wifiMgrTolerateBadRSSms = 300 * 1000; // 5m
unsigned long wifiMgrRescanInterval = 3600 * 1000; // 1h
unsigned long wifiMgrLastScan = 0;
unsigned long wifiMgrWaitForConnectMs = 30000; // 30s
unsigned long wifiMgrWaitForScanMs = 30000; // 30s
unsigned long wifiMgrScanCount = 0;
unsigned long wifiMgrConnectCount = 0;
uint8_t wifiMgrRebootAfterUnsuccessfullTries = 0;
uint8_t wifiMgrUnsuccessfullTries = 0;

int8_t badRSS = -70;
const char* wifiMgrSSID;
const char* wifiMgrPW;
const char* wifiMgrHN;

void (*loopFunctionPointer)(void) = nullptr;
void (*wifiMgrNotifyNoWifiCallback)(void) = nullptr;

XWebServer *wifiMgrServer;

boolean waitForWifi(unsigned long timeout) {
    unsigned long waitForConnectStart = millis();
    while (!WiFi.isConnected() && (millis() - waitForConnectStart) < timeout) {
        if (loopFunctionPointer != nullptr) loopFunctionPointer();
        delay(10);
    }
    return WiFi.isConnected();
}

void connectToWifi() {

#if defined(ESP8266)
    if (wifiMgrMdns.isRunning()) wifiMgrMdns.end();
#elif defined(ESP32)
#endif

    WiFi.scanDelete();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    wifiMgrScanCount++;

    int n = WiFi.scanNetworks(true, false, 0);

    unsigned long waitForScanStart = millis();

    while (WiFi.scanComplete() < 0 && (millis() - waitForScanStart) < wifiMgrWaitForScanMs) {
        if (loopFunctionPointer != nullptr) loopFunctionPointer();
        delay(10);
    }
    n = WiFi.scanComplete();

    if (n > 0) {
        String ssid;
        uint8_t encryptionType;
        int32_t RSSI;
        uint8_t *BSSID;
        int32_t channel;
        bool isHidden;

        uint8_t *bestBSSID = nullptr;
        int32_t bestRSSI = -999;
        int32_t bestChannel = 0;

        for (int i = 0; i < n; i++) {
#if defined(ESP8266)
            WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
#elif defined(ESP32)
            WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel);
	    isHidden = false;
#endif

            if (!isHidden && ssid.equals(wifiMgrSSID) && RSSI > bestRSSI) {
                bestRSSI = RSSI;
                bestBSSID = BSSID;
                bestChannel = channel;
            }
        }

        wifiMgrLastScan = millis();

        if (bestBSSID != nullptr) {
            WiFi.begin(wifiMgrSSID, wifiMgrPW, bestChannel, bestBSSID);
            wifiMgrConnectCount++;
            if (!waitForWifi(wifiMgrWaitForConnectMs)) {
                WiFi.disconnect(true);
                WiFi.mode(WIFI_OFF);
		        wifiMgrUnsuccessfullTries += 1;
		        if (wifiMgrUnsuccessfullTries >= wifiMgrRebootAfterUnsuccessfullTries) {
		            ESP.restart();
		        }
            } else {
		        wifiMgrUnsuccessfullTries = 0;
		        if (wifiMgrHN != nullptr) {
#if defined(ESP8266)
                    if (wifiMgrMdns.isRunning()) wifiMgrMdns.end();
                    wifiMgrMdns.begin(wifiMgrHN, WiFi.localIP());
#elif defined(ESP32)
		            mdns_init();
		            mdns_hostname_set(wifiMgrHN);
#endif
		        }
                wifiMgrLastNonShitRSS = millis();
            }
	    }
    }
}

void setupWifi(const char* SSID, const char* password) {
    setupWifi(SSID, password, nullptr);
}

void setupWifi(const char* SSID, const char* password, const char* hostname) {
    setupWifi(SSID, password, hostname, wifiMgrTolerateBadRSSms, wifiMgrWaitForConnectMs);
}

void setupWifi(const char* SSID, const char* password, const char* hostname, unsigned long tolerateBadRSSms, unsigned long waitForConnectMs) {
    setupWifi(SSID, password, hostname, tolerateBadRSSms, waitForConnectMs, wifiMgrWaitForScanMs, wifiMgrRescanInterval);
}

void onOTAEnd(bool success) {
  if (success) {
    ESP.restart();
  }
}

void setupWifi(const char* SSID, const char* password, const char* hostname, unsigned long tolerateBadRSSms, unsigned long waitForConnectMs, unsigned long waitForScanMs, unsigned long rescanInterval) {
    WiFi.mode(WIFI_STA);
    if (hostname != nullptr) WiFi.hostname(hostname);
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

#if defined(ESP8266)
    ESP8266WiFiClass::persistent(false);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#elif defined(ESP32)
    WiFi.persistent(false);
    WiFi.setSleep(false);
#endif


    wifiMgrSSID = SSID;
    wifiMgrPW = password;
    wifiMgrHN = hostname;
    wifiMgrTolerateBadRSSms = tolerateBadRSSms;
    wifiMgrWaitForConnectMs = waitForConnectMs;
    wifiMgrWaitForScanMs = waitForScanMs;
    wifiMgrRescanInterval = rescanInterval;

    connectToWifi();
}

void loopWifi() {
    if (!WiFi.isConnected()) {
        connectToWifi();
        if (wifiMgrNotifyNoWifiCallback != nullptr && millis() - wifiMgrlastConnected > wifiMgrNotifyNoWifiTimeout) {
            wifiMgrNotifyNoWifiCallback();
        }
    }
    if (WiFi.isConnected()) {
        if (millis() - wifiMgrlastConnected > 1000) {
            wifiMgrlastConnected = millis();

            int8_t rss = WiFi.RSSI();

            if (rss < badRSS) {
                if ((millis() - wifiMgrLastNonShitRSS) > wifiMgrTolerateBadRSSms) {
                    connectToWifi();
                }
            } else {
                wifiMgrLastNonShitRSS = millis();
            }

            if (wifiMgrRescanInterval > 0 && (millis() - wifiMgrLastScan) > wifiMgrRescanInterval) {
                connectToWifi();
            }
        }
    }
}

void sendRSSI() {
    wifiMgrServer->send(200, "text/plain", String(WiFi.RSSI()));
}

void isConnected() {
    wifiMgrServer->send(200, "text/plain", String(WiFi.isConnected()));
}

void ssid() {
    wifiMgrServer->send(200, "text/plain", WiFi.SSID());
}

void bssid() {
    wifiMgrServer->send(200, "text/plain", WiFi.BSSIDstr());
}

void status() {
    String s = "ssid: " + WiFi.SSID() + "\nconnected: " + String(WiFi.isConnected()) + "\nbssid: " + WiFi.BSSIDstr() + "\nrssi: " + String(WiFi.RSSI()) + "\nuptime: " + String(millis()/1000) + "s\nlast scan: " + String((millis() - wifiMgrLastScan)/1000) + "s\nscanned: " + String(wifiMgrScanCount) + " times\nconnected: " + String(wifiMgrConnectCount) + " times";
    wifiMgrServer->send(200, "text/plain", s);
}

void restart() {
    wifiMgrServer->send(200, "text/plain", "restarting");
    delay(500);
    ESP.restart();
}

void wifiMgrExpose(XWebServer *wifiMgrServer_) {
    wifiMgrServer = wifiMgrServer_;
    wifiMgrServer->on("/wifiMgr/rssi", sendRSSI);
    wifiMgrServer->on("/wifiMgr/isConnected", isConnected);
    wifiMgrServer->on("/wifiMgr/ssid", ssid);
    wifiMgrServer->on("/wifiMgr/bssid", bssid);
    wifiMgrServer->on("/wifiMgr/status", status);
    wifiMgrServer->on("/wifiMgr/restart", restart);

    ElegantOTA.begin(wifiMgrServer);
}

XWebServer* wifiMgrGetWebServer() {
    return wifiMgrServer;
}

void wifiMgrSetBadRSSI(int8_t rssi) {
    badRSS = rssi;
}

void wifiMgrSetRebootAfterUnsuccessfullTries(uint8_t _wifiMgrRebootAfterUnsuccessfullTries) {
    wifiMgrRebootAfterUnsuccessfullTries = _wifiMgrRebootAfterUnsuccessfullTries;
}

void wifiMgrNotifyNoWifi(void (*wifiMgrNotifyNoWifiCallbackArg)(void), unsigned long timeout) {
    wifiMgrNotifyNoWifiCallback = wifiMgrNotifyNoWifiCallbackArg;
    wifiMgrNotifyNoWifiTimeout = timeout;
}

void setLoopFunction(void (*loopFunctionPointerArg)(void)) {
    loopFunctionPointer = loopFunctionPointerArg;
}
