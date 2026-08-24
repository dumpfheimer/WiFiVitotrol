#include "vito_wifi.h"

XWebServer server(80);

//
//    THE HANDLERS
//    -> callbacks for URLs
//
void wifiHandleGetData() {
    String uri = server.uri();
    String datapointName = uri.substring(4);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        // TODO: iterate over read-only-datapoints
        server.send(404, "text/plain", "NOT FOUND");
        return;
    }

    server.send(200, "text/plain", String(datapoint->getFloatValue()));
}

void wifiHandleSetData() {
    String uri = server.uri();
    String datapointName = uri.substring(4);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        server.send(404, "text/plain", "NOT FOUND");
        return;
    }

    String strValue;
    float floatValue;
    bool hasValue = false;
    bool forceWrite = false;
    for (int i = 0; i < server.args(); i++) {
        if (server.argName(i) == "val") {
            strValue = server.arg(i);
            floatValue = strValue.toFloat();
            hasValue = true;
            break;
        }
        if (server.argName(i) == "force" || server.argName(i) == "forceWrite") {
            forceWrite = true;
        }
    }
    if (hasValue) {
        datapoint->setValueReceivedByWifi(floatValue, forceWrite);
        String ret = String(datapoint->getFloatValue());
        server.send(200, "text/plain", ret);
        notifyCommandReceived();
    } else {
        server.send(400, "text/plain", "val parameter is missing");
    }
}

void wifiHandleGetSource() {
    String uri = server.uri();
    String datapointName = uri.substring(10);
    debugPrintln("Requested URI ");
    debugPrintln(uri);
    debugPrintln(datapointName);

    WritableData *datapoint = getWritableDataByName(datapointName);

    if (datapoint == nullptr) {
        // TODO: iterate over read-only-datapoints
        server.send(404, "text/plain", "NOT FOUND");
        return;
    }

    switch (datapoint->getSource()) {
        case DataSource::WIFI:
            server.send(200, "text/plain", "WIFI");
            return;
        case DataSource::HEATER:
            server.send(200, "text/plain", "HEATER");
            return;
        default:
            server.send(200, "text/plain", "UNKNOWN");
            return;
    }
}

void wifiHandleGetLinkState() {
    server.send(200, "text/plain", linkState);
}

void wifiHandleGetOverview() {
    String ret;
    ret.reserve(8192);
    
    // Start HTML document with responsive design
    ret = "<!DOCTYPE html>\n";
    ret += "<html lang=\"en\">\n";
    ret += "<head>\n";
    ret += "  <meta charset=\"UTF-8\">\n";
    ret += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ret += "  <title>WiFiVitotrol Dashboard</title>\n";
    ret += "  <style>\n";
    ret += "    :root {\n";
    ret += "      --primary-color: #3498db;\n";
    ret += "      --secondary-color: #2c3e50;\n";
    ret += "      --success-color: #2ecc71;\n";
    ret += "      --danger-color: #e74c3c;\n";
    ret += "      --warning-color: #f39c12;\n";
    ret += "      --light-color: #ecf0f1;\n";
    ret += "      --dark-color: #34495e;\n";
    ret += "    }\n";
    ret += "    body {\n";
    ret += "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    ret += "      line-height: 1.6;\n";
    ret += "      color: #333;\n";
    ret += "      margin: 0;\n";
    ret += "      padding: 0;\n";
    ret += "      background-color: #f5f5f5;\n";
    ret += "    }\n";
    ret += "    .container {\n";
    ret += "      max-width: 1200px;\n";
    ret += "      margin: 0 auto;\n";
    ret += "      padding: 20px;\n";
    ret += "    }\n";
    ret += "    header {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "      padding: 1rem;\n";
    ret += "      text-align: center;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "    }\n";
    ret += "    h1, h2, h3 {\n";
    ret += "      margin-top: 0;\n";
    ret += "    }\n";
    ret += "    .card {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    ret += "      padding: 20px;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .card-header {\n";
    ret += "      font-size: 1.2rem;\n";
    ret += "      font-weight: bold;\n";
    ret += "      border-bottom: 1px solid #eee;\n";
    ret += "      padding-bottom: 10px;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "    }\n";
    ret += "    .status {\n";
    ret += "      display: inline-block;\n";
    ret += "      padding: 5px 10px;\n";
    ret += "      border-radius: 3px;\n";
    ret += "      font-weight: bold;\n";
    ret += "      margin-bottom: 10px;\n";
    ret += "    }\n";
    ret += "    .status-connected {\n";
    ret += "      background-color: var(--success-color);\n";
    ret += "      color: white;\n";
    ret += "    }\n";
    ret += "    .status-disconnected {\n";
    ret += "      background-color: var(--danger-color);\n";
    ret += "      color: white;\n";
    ret += "    }\n";
    ret += "    .status-warning {\n";
    ret += "      background-color: var(--warning-color);\n";
    ret += "      color: white;\n";
    ret += "    }\n";
    ret += "    .grid {\n";
    ret += "      display: grid;\n";
    ret += "      grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));\n";
    ret += "      gap: 20px;\n";
    ret += "    }\n";
    ret += "    .info-item {\n";
    ret += "      margin-bottom: 8px;\n";
    ret += "    }\n";
    ret += "    .info-label {\n";
    ret += "      font-weight: bold;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "    }\n";
    ret += "    .buffer-content {\n";
    ret += "      font-family: monospace;\n";
    ret += "      background-color: #f8f9fa;\n";
    ret += "      padding: 10px;\n";
    ret += "      border-radius: 3px;\n";
    ret += "      overflow-x: auto;\n";
    ret += "      font-size: 0.9rem;\n";
    ret += "    }\n";
    ret += "    .data-table {\n";
    ret += "      width: 100%;\n";
    ret += "      border-collapse: collapse;\n";
    ret += "      margin-top: 10px;\n";
    ret += "    }\n";
    ret += "    .data-table th, .data-table td {\n";
    ret += "      padding: 8px;\n";
    ret += "      text-align: left;\n";
    ret += "      border-bottom: 1px solid #ddd;\n";
    ret += "    }\n";
    ret += "    .data-table th {\n";
    ret += "      background-color: var(--light-color);\n";
    ret += "      color: var(--dark-color);\n";
    ret += "    }\n";
    ret += "    .nav {\n";
    ret += "      display: flex;\n";
    ret += "      background-color: var(--secondary-color);\n";
    ret += "      border-radius: 5px;\n";
    ret += "      overflow: hidden;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .nav a {\n";
    ret += "      color: white;\n";
    ret += "      text-decoration: none;\n";
    ret += "      padding: 10px 15px;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .nav a:hover {\n";
    ret += "      background-color: rgba(255,255,255,0.1);\n";
    ret += "    }\n";
    ret += "    .nav a.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "    }\n";
    ret += "    @media (max-width: 768px) {\n";
    ret += "      .grid {\n";
    ret += "        grid-template-columns: 1fr;\n";
    ret += "      }\n";
    ret += "      .nav {\n";
    ret += "        flex-direction: column;\n";
    ret += "      }\n";
    ret += "    }\n";
    ret += "  </style>\n";
    ret += "</head>\n";
    ret += "<body>\n";
    ret += "  <div class=\"container\">\n";
    ret += "    <header>\n";
    ret += "      <h1>WiFiVitotrol Dashboard</h1>\n";
    ret += "      <p>Build: ";
    ret += __DATE__;
    ret += " ";
    ret += __TIME__;
    ret += "</p>\n";
    ret += "    </header>\n";
    
    ret += "    <div class=\"nav\">\n";
    ret += "      <a href=\"/\" class=\"active\">Overview</a>\n";
    ret += "      <a href=\"/log\">Logs</a>\n";
    ret += "      <a href=\"/datasets\">Datasets</a>\n";
    ret += "      <a href=\"/registers\">Registers</a>\n";
    ret += "    </div>\n";
    
    // System Status Card
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">System Status</div>\n";
    
    // Heater connection status
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        ret += "      <div class=\"status status-connected\">Heater is connected</div>\n";
    } else {
        ret += "      <div class=\"status status-disconnected\">Heater is not connected</div>\n";
    }
    
    // Communication status
    if (preventCommunication()) {
        ret += "      <div class=\"status status-warning\">Communication is prevented</div>\n";
    } else {
        ret += "      <div class=\"status status-connected\">Communication is active</div>\n";
    }
    
    // Device ID information
#ifdef FIND_DEVICE_ID
    if (deviceIdFound) {
        ret += "      <div class=\"info-item\"><span class=\"info-label\">DeviceID:</span> 0x";
        ret += String(getRegisterValue(0xF9), HEX);
        ret += "</div>\n";
    } else {
        ret += "      <div class=\"info-item\"><span class=\"info-label\">Scanning for DeviceID:</span> Currently at 0x";
        ret += String(getRegisterValue(0xF9), HEX);
        ret += "</div>\n";
    }
#endif

    ret += "      <div class=\"info-item\"><span class=\"info-label\">Link state:</span> ";
    ret += String(linkState);
    ret += "</div>\n";
    
    unsigned long m = millis();
    
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Uptime:</span> ";
    ret += String(m/1000);
    ret += " seconds</div>\n";
    ret += "    </div>\n";
    
    // Timing Information Card
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Timing Information</div>\n";
    ret += "      <div class=\"grid\">\n";
    ret += "        <div>\n";
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last communication:</span> ";
    ret += String(m - lastHeaterCommandReceivedAt);
    ret += " ms</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last read:</span> ";
    ret += String(m - lastReadAt);
    ret += " ms (";
    ret += String(readByte, HEX);
    ret += ")</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last message:</span> ";
    ret += String(m - lastMessageAt);
    ret += " ms</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last message - last read:</span> ";
    ret += String(lastMessageAt - lastReadAt);
    ret += " ms</div>\n";
    ret += "        </div>\n";
    
    ret += "        <div>\n";
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last valid message:</span> ";
    ret += String(m - lastValidMessageAt);
    ret += " ms</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last valid broadcast message:</span> ";
    ret += String(m - lastBroadcastMessage);
    ret += " ms</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last valid message with response:</span> ";
    ret += String(m - lastMessageWithResponseAt);
    ret += " ms</div>\n";
    
    ret += "          <div class=\"info-item\"><span class=\"info-label\">Last valid message without response:</span> ";
    ret += String(m - lastMessageWithoutResponseAt);
    ret += " ms</div>\n";
    ret += "        </div>\n";
    ret += "      </div>\n";
    ret += "    </div>\n";
    
    // Buffer Information Card
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Buffer Information</div>\n";
    
    // Receive Buffer
    ret += "      <h3>Receive Buffer</h3>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Length:</span> ";
    ret += String(buffer[3]);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Pointer:</span> ";
    ret += String((unsigned) &buffer, HEX);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Content:</span></div>\n";
    ret += "      <div class=\"buffer-content\">";
    
    // Use a safer approach for buffer content
    uint8_t bufLen = buffer[3];
    if (bufLen > BUFFER_LEN) bufLen = BUFFER_LEN; // Safety check
    for (int i = 0; i < bufLen; i++) {
        ret += String(buffer[i], HEX);
        ret += "-";
    }
    ret += "</div>\n";
    
    // Last Valid Message
    ret += "      <h3>Last Valid Message</h3>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Length:</span> ";
    ret += String(lastValidMessage[3]);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Pointer:</span> ";
    ret += String((unsigned) &lastValidMessage, HEX);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Content:</span></div>\n";
    ret += "      <div class=\"buffer-content\">";
    
    bufLen = lastValidMessage[3];
    if (bufLen > BUFFER_LEN) bufLen = BUFFER_LEN; // Safety check
    for (int i = 0; i < bufLen; i++) {
        ret += String(lastValidMessage[i], HEX);
        ret += "-";
    }
    ret += "</div>\n";
    
    // Last Invalid Message
    ret += "      <h3>Last Invalid Message</h3>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Length:</span> ";
    ret += String(lastInvalidMessage[3]);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Pointer:</span> ";
    ret += String((unsigned) &lastInvalidMessage, HEX);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Content:</span></div>\n";
    ret += "      <div class=\"buffer-content\">";
    
    bufLen = lastInvalidMessage[3];
    if (bufLen > BUFFER_LEN) bufLen = BUFFER_LEN; // Safety check
    for (int i = 0; i < bufLen; i++) {
        ret += String(lastInvalidMessage[i], HEX);
        ret += "-";
    }
    ret += "</div>\n";
    
    // Response Buffer
    ret += "      <h3>Response Buffer</h3>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Length:</span> ";
    ret += String(responseBuffer[3]);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Pointer:</span> ";
    ret += String((unsigned) &responseBuffer, HEX);
    ret += "</div>\n";
    ret += "      <div class=\"info-item\"><span class=\"info-label\">Content:</span></div>\n";
    ret += "      <div class=\"buffer-content\">";
    
    bufLen = responseBuffer[3];
    if (bufLen > BUFFER_LEN) bufLen = BUFFER_LEN; // Safety check
    for (int i = 0; i < bufLen; i++) {
        ret += String(responseBuffer[i], HEX);
        ret += "-";
    }
    ret += "</div>\n";
    ret += "    </div>\n";
    
    // Data Points Card
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Data Points</div>\n";
    ret += "      <table class=\"data-table\">\n";
    ret += "        <thead>\n";
    ret += "          <tr>\n";
    ret += "            <th>Name</th>\n";
    ret += "            <th>Value</th>\n";
    ret += "            <th>Status</th>\n";
    ret += "            <th>Last Activity</th>\n";
    ret += "          </tr>\n";
    ret += "        </thead>\n";
    ret += "        <tbody>\n";

    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        ret += "          <tr>\n";
        ret += "            <td>";
        ret += String(writableDataPoint[p]->getName());
        ret += "</td>\n";
        ret += "            <td>";
        ret += String(writableDataPoint[p]->getValue());
        ret += "</td>\n";
        ret += "            <td>";
        if (writableDataPoint[p]->getHasValidValue()) {
            ret += "Valid";
        } else {
            ret += "Invalid";
        }
        if (writableDataPoint[p]->wantsToSendValue()) {
            ret += ", Write pending";
        }
        if (writableDataPoint[p]->preventCommunication()) {
            ret += ", Preventing comm";
        }
        ret += "</td>\n";
        ret += "            <td>Last send: ";
        ret += String(m - writableDataPoint[p]->getLastSend());
        ret += "ms, Last set: ";
        ret += String(m - writableDataPoint[p]->getLastSet());
        ret += "ms</td>\n";
        ret += "          </tr>\n";
        p++;
    }

    ret += "        </tbody>\n";
    ret += "      </table>\n";
    ret += "    </div>\n";
    
    // Close HTML tags
    ret += "  </div>\n";
    ret += "</body>\n";
    ret += "</html>";

    server.send(200, "text/html", ret);
}

void wifiHandleGetLog() {
    String ret;
    ret.reserve(4096);
    
    // Start HTML document with responsive design
    ret = "<!DOCTYPE html>\n";
    ret += "<html lang=\"en\">\n";
    ret += "<head>\n";
    ret += "  <meta charset=\"UTF-8\">\n";
    ret += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ret += "  <title>WiFiVitotrol - Logs</title>\n";
    ret += "  <style>\n";
    ret += "    :root {\n";
    ret += "      --primary-color: #3498db;\n";
    ret += "      --secondary-color: #2c3e50;\n";
    ret += "      --success-color: #2ecc71;\n";
    ret += "      --danger-color: #e74c3c;\n";
    ret += "      --warning-color: #f39c12;\n";
    ret += "      --light-color: #ecf0f1;\n";
    ret += "      --dark-color: #34495e;\n";
    ret += "    }\n";
    ret += "    body {\n";
    ret += "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    ret += "      line-height: 1.6;\n";
    ret += "      color: #333;\n";
    ret += "      margin: 0;\n";
    ret += "      padding: 0;\n";
    ret += "      background-color: #f5f5f5;\n";
    ret += "    }\n";
    ret += "    .container {\n";
    ret += "      max-width: 1200px;\n";
    ret += "      margin: 0 auto;\n";
    ret += "      padding: 20px;\n";
    ret += "    }\n";
    ret += "    header {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "      padding: 1rem;\n";
    ret += "      text-align: center;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "    }\n";
    ret += "    h1, h2, h3 {\n";
    ret += "      margin-top: 0;\n";
    ret += "    }\n";
    ret += "    .card {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    ret += "      padding: 20px;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .card-header {\n";
    ret += "      font-size: 1.2rem;\n";
    ret += "      font-weight: bold;\n";
    ret += "      border-bottom: 1px solid #eee;\n";
    ret += "      padding-bottom: 10px;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "    }\n";
    ret += "    .nav {\n";
    ret += "      display: flex;\n";
    ret += "      background-color: var(--secondary-color);\n";
    ret += "      border-radius: 5px;\n";
    ret += "      overflow: hidden;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .nav a {\n";
    ret += "      color: white;\n";
    ret += "      text-decoration: none;\n";
    ret += "      padding: 10px 15px;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .nav a:hover {\n";
    ret += "      background-color: rgba(255,255,255,0.1);\n";
    ret += "    }\n";
    ret += "    .nav a.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "    }\n";
    ret += "    .log-table {\n";
    ret += "      width: 100%;\n";
    ret += "      border-collapse: collapse;\n";
    ret += "      margin-top: 10px;\n";
    ret += "      font-family: monospace;\n";
    ret += "    }\n";
    ret += "    .log-table th, .log-table td {\n";
    ret += "      padding: 8px;\n";
    ret += "      text-align: left;\n";
    ret += "      border-bottom: 1px solid #ddd;\n";
    ret += "    }\n";
    ret += "    .log-table th {\n";
    ret += "      background-color: var(--light-color);\n";
    ret += "      color: var(--dark-color);\n";
    ret += "    }\n";
    ret += "    .log-table tr:nth-child(even) {\n";
    ret += "      background-color: #f8f9fa;\n";
    ret += "    }\n";
    ret += "    .log-table tr:hover {\n";
    ret += "      background-color: #e9ecef;\n";
    ret += "    }\n";
    ret += "    .log-index {\n";
    ret += "      font-weight: bold;\n";
    ret += "      width: 50px;\n";
    ret += "    }\n";
    ret += "    .log-null {\n";
    ret += "      color: #999;\n";
    ret += "      font-style: italic;\n";
    ret += "    }\n";
    ret += "    @media (max-width: 768px) {\n";
    ret += "      .nav {\n";
    ret += "        flex-direction: column;\n";
    ret += "      }\n";
    ret += "    }\n";
    ret += "  </style>\n";
    ret += "</head>\n";
    ret += "<body>\n";
    ret += "  <div class=\"container\">\n";
    ret += "    <header>\n";
    ret += "      <h1>WiFiVitotrol - System Logs</h1>\n";
    ret += "    </header>\n";
    
    ret += "    <div class=\"nav\">\n";
    ret += "      <a href=\"/\">Overview</a>\n";
    ret += "      <a href=\"/log\" class=\"active\">Logs</a>\n";
    ret += "      <a href=\"/datasets\">Datasets</a>\n";
    ret += "      <a href=\"/registers\">Registers</a>\n";
    ret += "    </div>\n";
    
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Message Log (most recent first)</div>\n";
    ret += "      <table class=\"log-table\">\n";
    ret += "        <thead>\n";
    ret += "          <tr>\n";
    ret += "            <th class=\"log-index\">Index</th>\n";
    ret += "            <th>Message</th>\n";
    ret += "          </tr>\n";
    ret += "        </thead>\n";
    ret += "        <tbody>\n";
    
    for (int8_t i = 0; i < MSG_LOG_SIZE; i++) {
        const char* ptr = getLastMessage(i);
        ret += "          <tr>\n";
        ret += "            <td class=\"log-index\">";
        if (i < 10) ret += "0";
        ret += String(i);
        ret += "</td>\n";
        ret += "            <td>";
        if (ptr != nullptr) {
            ret += String(ptr);
        } else {
            ret += "<span class=\"log-null\">null</span>";
        }
        ret += "</td>\n";
        ret += "          </tr>\n";
    }
    
    ret += "        </tbody>\n";
    ret += "      </table>\n";
    ret += "    </div>\n";
    
    // Close HTML tags
    ret += "  </div>\n";
    ret += "</body>\n";
    ret += "</html>";

    server.send(200, "text/html", ret);
}

void wifiHandleReadDataset() {
    if (!server.hasArg("id")) {
        server.send(500, "text/plain", "id paramater missing");
        return;
    }
    int id = server.arg("id").toInt();
    String ret = "{ ";
    Dataset *d = getDataset(id);
    if (d != nullptr) {
        ret += "\n" + String(id) + ": [";
        for (int i2 = 0; i2 < d->len; i2++) {
            ret += String(d->data[i2]);
            if (i2+1 < d->len) ret += ",";
        }
        ret += "]";
    }
    ret += "\n}";
    server.send(200, "application/json", ret);
}
void wifiHandleReadDatasets() {
    String ret;
    ret.reserve(8192);
    
    // Start HTML document with responsive design
    ret = "<!DOCTYPE html>\n";
    ret += "<html lang=\"en\">\n";
    ret += "<head>\n";
    ret += "  <meta charset=\"UTF-8\">\n";
    ret += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ret += "  <title>WiFiVitotrol - Datasets</title>\n";
    ret += "  <style>\n";
    ret += "    :root {\n";
    ret += "      --primary-color: #3498db;\n";
    ret += "      --secondary-color: #2c3e50;\n";
    ret += "      --success-color: #2ecc71;\n";
    ret += "      --danger-color: #e74c3c;\n";
    ret += "      --warning-color: #f39c12;\n";
    ret += "      --light-color: #ecf0f1;\n";
    ret += "      --dark-color: #34495e;\n";
    ret += "    }\n";
    ret += "    body {\n";
    ret += "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    ret += "      line-height: 1.6;\n";
    ret += "      color: #333;\n";
    ret += "      margin: 0;\n";
    ret += "      padding: 0;\n";
    ret += "      background-color: #f5f5f5;\n";
    ret += "    }\n";
    ret += "    .container {\n";
    ret += "      max-width: 1200px;\n";
    ret += "      margin: 0 auto;\n";
    ret += "      padding: 20px;\n";
    ret += "    }\n";
    ret += "    header {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "      padding: 1rem;\n";
    ret += "      text-align: center;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "    }\n";
    ret += "    h1, h2, h3 {\n";
    ret += "      margin-top: 0;\n";
    ret += "    }\n";
    ret += "    .card {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    ret += "      padding: 20px;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .card-header {\n";
    ret += "      font-size: 1.2rem;\n";
    ret += "      font-weight: bold;\n";
    ret += "      border-bottom: 1px solid #eee;\n";
    ret += "      padding-bottom: 10px;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "    }\n";
    ret += "    .nav {\n";
    ret += "      display: flex;\n";
    ret += "      background-color: var(--secondary-color);\n";
    ret += "      border-radius: 5px;\n";
    ret += "      overflow: hidden;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .nav a {\n";
    ret += "      color: white;\n";
    ret += "      text-decoration: none;\n";
    ret += "      padding: 10px 15px;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .nav a:hover {\n";
    ret += "      background-color: rgba(255,255,255,0.1);\n";
    ret += "    }\n";
    ret += "    .nav a.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "    }\n";
    ret += "    .dataset-grid {\n";
    ret += "      display: grid;\n";
    ret += "      grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));\n";
    ret += "      gap: 20px;\n";
    ret += "    }\n";
    ret += "    .dataset-card {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    ret += "      padding: 15px;\n";
    ret += "      transition: transform 0.2s, box-shadow 0.2s;\n";
    ret += "    }\n";
    ret += "    .dataset-card:hover {\n";
    ret += "      transform: translateY(-3px);\n";
    ret += "      box-shadow: 0 4px 8px rgba(0,0,0,0.15);\n";
    ret += "    }\n";
    ret += "    .dataset-header {\n";
    ret += "      font-weight: bold;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "      margin-bottom: 10px;\n";
    ret += "      border-bottom: 1px solid #eee;\n";
    ret += "      padding-bottom: 5px;\n";
    ret += "    }\n";
    ret += "    .dataset-content {\n";
    ret += "      font-family: monospace;\n";
    ret += "      background-color: #f8f9fa;\n";
    ret += "      padding: 10px;\n";
    ret += "      border-radius: 3px;\n";
    ret += "      overflow-x: auto;\n";
    ret += "      max-height: 200px;\n";
    ret += "      overflow-y: auto;\n";
    ret += "    }\n";
    ret += "    .dataset-empty {\n";
    ret += "      color: #999;\n";
    ret += "      font-style: italic;\n";
    ret += "      text-align: center;\n";
    ret += "      padding: 20px;\n";
    ret += "    }\n";
    ret += "    .json-view {\n";
    ret += "      display: none;\n";
    ret += "      font-family: monospace;\n";
    ret += "      white-space: pre;\n";
    ret += "      background-color: #f8f9fa;\n";
    ret += "      padding: 15px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      margin-top: 20px;\n";
    ret += "      overflow-x: auto;\n";
    ret += "    }\n";
    ret += "    .view-toggle {\n";
    ret += "      display: flex;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button {\n";
    ret += "      background-color: var(--light-color);\n";
    ret += "      border: none;\n";
    ret += "      padding: 8px 15px;\n";
    ret += "      cursor: pointer;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .view-toggle button:first-child {\n";
    ret += "      border-top-left-radius: 3px;\n";
    ret += "      border-bottom-left-radius: 3px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button:last-child {\n";
    ret += "      border-top-right-radius: 3px;\n";
    ret += "      border-bottom-right-radius: 3px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "    }\n";
    ret += "    @media (max-width: 768px) {\n";
    ret += "      .dataset-grid {\n";
    ret += "        grid-template-columns: 1fr;\n";
    ret += "      }\n";
    ret += "      .nav {\n";
    ret += "        flex-direction: column;\n";
    ret += "      }\n";
    ret += "    }\n";
    ret += "  </style>\n";
    ret += "</head>\n";
    ret += "<body>\n";
    ret += "  <div class=\"container\">\n";
    ret += "    <header>\n";
    ret += "      <h1>WiFiVitotrol - Datasets</h1>\n";
    ret += "    </header>\n";
    
    ret += "    <div class=\"nav\">\n";
    ret += "      <a href=\"/\">Overview</a>\n";
    ret += "      <a href=\"/log\">Logs</a>\n";
    ret += "      <a href=\"/datasets\" class=\"active\">Datasets</a>\n";
    ret += "      <a href=\"/registers\">Registers</a>\n";
    ret += "    </div>\n";
    
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Available Datasets</div>\n";
    ret += "      <div class=\"view-toggle\">\n";
    ret += "        <button id=\"grid-view-btn\" class=\"active\" onclick=\"toggleView('grid')\">Grid View</button>\n";
    ret += "        <button id=\"json-view-btn\" onclick=\"toggleView('json')\">JSON View</button>\n";
    ret += "      </div>\n";
    
    // Prepare JSON data for the JSON view
    String jsonData = "{ ";
    bool hasDatasets = false;
    
    ret += "      <div id=\"grid-view\" class=\"dataset-grid\">\n";
    
    for(int i = 0; i < DATASETS; i++) {
        Dataset *d = getDataset(i);
        if (d != nullptr) {
            hasDatasets = true;
            
            // Add to grid view
            ret += "        <div class=\"dataset-card\">\n";
            ret += "          <div class=\"dataset-header\">Dataset " + String(i) + "</div>\n";
            ret += "          <div class=\"dataset-content\">[";
            
            for (int i2 = 0; i2 < d->len; i2++) {
                ret += String(d->data[i2]);
                if (i2+1 < d->len) ret += ", ";
            }
            
            ret += "]</div>\n";
            ret += "        </div>\n";
            
            // Add to JSON data
            jsonData += "\n  \"" + String(i) + "\": [";
            for (int i2 = 0; i2 < d->len; i2++) {
                jsonData += String(d->data[i2]);
                if (i2+1 < d->len) jsonData += ", ";
            }
            jsonData += "],";
        }
    }
    
    if (!hasDatasets) {
        ret += "        <div class=\"dataset-empty\">No datasets available</div>\n";
    }
    
    ret += "      </div>\n";
    
    // Finalize JSON data
    jsonData += "\n}";
    if (hasDatasets) {
        jsonData.setCharAt(jsonData.length() - 2, ' ');
    }
    
    // Add JSON view
    ret += "      <div id=\"json-view\" class=\"json-view\">" + jsonData + "</div>\n";
    ret += "    </div>\n";
    
    // Add JavaScript for view toggling
    ret += "    <script>\n";
    ret += "      function toggleView(viewType) {\n";
    ret += "        const gridView = document.getElementById('grid-view');\n";
    ret += "        const jsonView = document.getElementById('json-view');\n";
    ret += "        const gridBtn = document.getElementById('grid-view-btn');\n";
    ret += "        const jsonBtn = document.getElementById('json-view-btn');\n";
    ret += "        \n";
    ret += "        if (viewType === 'grid') {\n";
    ret += "          gridView.style.display = 'grid';\n";
    ret += "          jsonView.style.display = 'none';\n";
    ret += "          gridBtn.classList.add('active');\n";
    ret += "          jsonBtn.classList.remove('active');\n";
    ret += "        } else {\n";
    ret += "          gridView.style.display = 'none';\n";
    ret += "          jsonView.style.display = 'block';\n";
    ret += "          gridBtn.classList.remove('active');\n";
    ret += "          jsonBtn.classList.add('active');\n";
    ret += "        }\n";
    ret += "      }\n";
    ret += "    </script>\n";
    
    // Close HTML tags
    ret += "  </div>\n";
    ret += "</body>\n";
    ret += "</html>";

    server.send(200, "text/html", ret);
    
    // Also provide JSON endpoint for API access
    server.on("/api/datasets", []() {
        String jsonData = "{ ";
        for(int i = 0; i < DATASETS; i++) {
            Dataset *d = getDataset(i);
            if (d != nullptr) {
                jsonData += "\"" + String(i) + "\": [";
                for (int i2 = 0; i2 < d->len; i2++) {
                    jsonData += String(d->data[i2]);
                    if (i2+1 < d->len) jsonData += ",";
                }
                jsonData += "],";
            }
        }
        jsonData += "}";
        if (jsonData.length() > 2) {
            jsonData.setCharAt(jsonData.length() - 2, ' ');
        }
        server.send(200, "application/json", jsonData);
    });
}

void wifiHandleReadRegisters() {
    String ret;
    ret.reserve(8192);
    
    // Start HTML document with responsive design
    ret = "<!DOCTYPE html>\n";
    ret += "<html lang=\"en\">\n";
    ret += "<head>\n";
    ret += "  <meta charset=\"UTF-8\">\n";
    ret += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ret += "  <title>WiFiVitotrol - Registers</title>\n";
    ret += "  <style>\n";
    ret += "    :root {\n";
    ret += "      --primary-color: #3498db;\n";
    ret += "      --secondary-color: #2c3e50;\n";
    ret += "      --success-color: #2ecc71;\n";
    ret += "      --danger-color: #e74c3c;\n";
    ret += "      --warning-color: #f39c12;\n";
    ret += "      --light-color: #ecf0f1;\n";
    ret += "      --dark-color: #34495e;\n";
    ret += "    }\n";
    ret += "    body {\n";
    ret += "      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
    ret += "      line-height: 1.6;\n";
    ret += "      color: #333;\n";
    ret += "      margin: 0;\n";
    ret += "      padding: 0;\n";
    ret += "      background-color: #f5f5f5;\n";
    ret += "    }\n";
    ret += "    .container {\n";
    ret += "      max-width: 1200px;\n";
    ret += "      margin: 0 auto;\n";
    ret += "      padding: 20px;\n";
    ret += "    }\n";
    ret += "    header {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "      padding: 1rem;\n";
    ret += "      text-align: center;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "    }\n";
    ret += "    h1, h2, h3 {\n";
    ret += "      margin-top: 0;\n";
    ret += "    }\n";
    ret += "    .card {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      box-shadow: 0 2px 5px rgba(0,0,0,0.1);\n";
    ret += "      padding: 20px;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .card-header {\n";
    ret += "      font-size: 1.2rem;\n";
    ret += "      font-weight: bold;\n";
    ret += "      border-bottom: 1px solid #eee;\n";
    ret += "      padding-bottom: 10px;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "    }\n";
    ret += "    .nav {\n";
    ret += "      display: flex;\n";
    ret += "      background-color: var(--secondary-color);\n";
    ret += "      border-radius: 5px;\n";
    ret += "      overflow: hidden;\n";
    ret += "      margin-bottom: 20px;\n";
    ret += "    }\n";
    ret += "    .nav a {\n";
    ret += "      color: white;\n";
    ret += "      text-decoration: none;\n";
    ret += "      padding: 10px 15px;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .nav a:hover {\n";
    ret += "      background-color: rgba(255,255,255,0.1);\n";
    ret += "    }\n";
    ret += "    .nav a.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "    }\n";
    ret += "    .register-grid {\n";
    ret += "      display: grid;\n";
    ret += "      grid-template-columns: repeat(auto-fill, minmax(60px, 1fr));\n";
    ret += "      gap: 10px;\n";
    ret += "    }\n";
    ret += "    .register-cell {\n";
    ret += "      background: white;\n";
    ret += "      border-radius: 3px;\n";
    ret += "      box-shadow: 0 1px 3px rgba(0,0,0,0.1);\n";
    ret += "      padding: 8px;\n";
    ret += "      text-align: center;\n";
    ret += "      position: relative;\n";
    ret += "      transition: transform 0.2s, box-shadow 0.2s;\n";
    ret += "    }\n";
    ret += "    .register-cell:hover {\n";
    ret += "      transform: translateY(-2px);\n";
    ret += "      box-shadow: 0 3px 6px rgba(0,0,0,0.15);\n";
    ret += "      z-index: 1;\n";
    ret += "    }\n";
    ret += "    .register-addr {\n";
    ret += "      font-size: 0.8rem;\n";
    ret += "      color: var(--secondary-color);\n";
    ret += "      font-weight: bold;\n";
    ret += "    }\n";
    ret += "    .register-value {\n";
    ret += "      font-family: monospace;\n";
    ret += "      font-size: 1rem;\n";
    ret += "      margin-top: 5px;\n";
    ret += "    }\n";
    ret += "    .register-value-dec {\n";
    ret += "      font-size: 0.8rem;\n";
    ret += "      color: #666;\n";
    ret += "    }\n";
    ret += "    .register-row-header {\n";
    ret += "      grid-column: 1 / -1;\n";
    ret += "      background-color: var(--light-color);\n";
    ret += "      color: var(--dark-color);\n";
    ret += "      padding: 5px 10px;\n";
    ret += "      font-weight: bold;\n";
    ret += "      border-radius: 3px;\n";
    ret += "      margin-top: 10px;\n";
    ret += "      margin-bottom: 5px;\n";
    ret += "    }\n";
    ret += "    .view-toggle {\n";
    ret += "      display: flex;\n";
    ret += "      margin-bottom: 15px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button {\n";
    ret += "      background-color: var(--light-color);\n";
    ret += "      border: none;\n";
    ret += "      padding: 8px 15px;\n";
    ret += "      cursor: pointer;\n";
    ret += "      transition: background-color 0.3s;\n";
    ret += "    }\n";
    ret += "    .view-toggle button:first-child {\n";
    ret += "      border-top-left-radius: 3px;\n";
    ret += "      border-bottom-left-radius: 3px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button:last-child {\n";
    ret += "      border-top-right-radius: 3px;\n";
    ret += "      border-bottom-right-radius: 3px;\n";
    ret += "    }\n";
    ret += "    .view-toggle button.active {\n";
    ret += "      background-color: var(--primary-color);\n";
    ret += "      color: white;\n";
    ret += "    }\n";
    ret += "    .json-view {\n";
    ret += "      display: none;\n";
    ret += "      font-family: monospace;\n";
    ret += "      white-space: pre;\n";
    ret += "      background-color: #f8f9fa;\n";
    ret += "      padding: 15px;\n";
    ret += "      border-radius: 5px;\n";
    ret += "      margin-top: 20px;\n";
    ret += "      overflow-x: auto;\n";
    ret += "    }\n";
    ret += "    @media (max-width: 768px) {\n";
    ret += "      .register-grid {\n";
    ret += "        grid-template-columns: repeat(auto-fill, minmax(50px, 1fr));\n";
    ret += "      }\n";
    ret += "      .nav {\n";
    ret += "        flex-direction: column;\n";
    ret += "      }\n";
    ret += "    }\n";
    ret += "  </style>\n";
    ret += "</head>\n";
    ret += "<body>\n";
    ret += "  <div class=\"container\">\n";
    ret += "    <header>\n";
    ret += "      <h1>WiFiVitotrol - Registers</h1>\n";
    ret += "    </header>\n";
    
    ret += "    <div class=\"nav\">\n";
    ret += "      <a href=\"/\">Overview</a>\n";
    ret += "      <a href=\"/log\">Logs</a>\n";
    ret += "      <a href=\"/datasets\">Datasets</a>\n";
    ret += "      <a href=\"/registers\" class=\"active\">Registers</a>\n";
    ret += "    </div>\n";
    
    ret += "    <div class=\"card\">\n";
    ret += "      <div class=\"card-header\">Register Values</div>\n";
    ret += "      <div class=\"view-toggle\">\n";
    ret += "        <button id=\"grid-view-btn\" class=\"active\" onclick=\"toggleView('grid')\">Grid View</button>\n";
    ret += "        <button id=\"json-view-btn\" onclick=\"toggleView('json')\">JSON View</button>\n";
    ret += "      </div>\n";
    
    // Prepare JSON data for the JSON view
    String jsonData = "[";
    for (int i = 0; i <= 255; i++) {
        jsonData += String((uint8_t) getRegisterValue(i));
        if (i < 255) jsonData += ", ";
    }
    jsonData += "]";
    
    ret += "      <div id=\"grid-view\" class=\"register-grid\">\n";
    
    // Add registers in a grid with row headers for each 16 registers
    for (int row = 0; row < 16; row++) {
        ret += "        <div class=\"register-row-header\">0x" + String(row * 16, HEX) + " - 0x" + String(row * 16 + 15, HEX) + "</div>\n";
        
        for (int col = 0; col < 16; col++) {
            int regAddr = row * 16 + col;
            uint8_t regValue = getRegisterValue(regAddr);
            
            ret += "        <div class=\"register-cell\">\n";
            ret += "          <div class=\"register-addr\">0x" + String(regAddr, HEX) + "</div>\n";
            ret += "          <div class=\"register-value\">0x" + String(regValue, HEX) + "</div>\n";
            ret += "          <div class=\"register-value-dec\">" + String(regValue) + "</div>\n";
            ret += "        </div>\n";
        }
    }
    
    ret += "      </div>\n";
    
    // Add JSON view
    ret += "      <div id=\"json-view\" class=\"json-view\">" + jsonData + "</div>\n";
    ret += "    </div>\n";
    
    // Add JavaScript for view toggling
    ret += "    <script>\n";
    ret += "      function toggleView(viewType) {\n";
    ret += "        const gridView = document.getElementById('grid-view');\n";
    ret += "        const jsonView = document.getElementById('json-view');\n";
    ret += "        const gridBtn = document.getElementById('grid-view-btn');\n";
    ret += "        const jsonBtn = document.getElementById('json-view-btn');\n";
    ret += "        \n";
    ret += "        if (viewType === 'grid') {\n";
    ret += "          gridView.style.display = 'grid';\n";
    ret += "          jsonView.style.display = 'none';\n";
    ret += "          gridBtn.classList.add('active');\n";
    ret += "          jsonBtn.classList.remove('active');\n";
    ret += "        } else {\n";
    ret += "          gridView.style.display = 'none';\n";
    ret += "          jsonView.style.display = 'block';\n";
    ret += "          gridBtn.classList.remove('active');\n";
    ret += "          jsonBtn.classList.add('active');\n";
    ret += "        }\n";
    ret += "      }\n";
    ret += "    </script>\n";
    
    // Close HTML tags
    ret += "  </div>\n";
    ret += "</body>\n";
    ret += "</html>";

    server.send(200, "text/html", ret);
    
    // Also provide JSON endpoint for API access
    server.on("/api/registers", []() {
        String jsonData = "[";
        for (int i = 0; i <= 255; i++) {
            jsonData += String((uint8_t) getRegisterValue(i));
            if (i < 255) jsonData += ",";
        }
        jsonData += "]";
        server.send(200, "application/json", jsonData);
    });
}

void wifiHandleIsPreventCommunication() {
    if (preventCommunication()) {
        server.send(200, "text/plain", "true");
    } else {
        server.send(200, "text/plain", "false");
    }
}

void wifiHandleIsConnected() {
    if (lastHeaterCommandReceivedAt > 0 && millis() - lastHeaterCommandReceivedAt < 60000) {
        server.send(200, "text/plain", "true");
    } else {
        server.send(200, "text/plain", "false");
    }
}

void wifiHandleRequestDataset() {
    if (server.hasArg("id")) requestDataset = server.arg("id").toInt();
    server.send(200, "text/plain", String(requestDataset));
}

void wifiHandleGetRequestDataset() {
    server.send(200, "text/plain", String(requestDataset));
}

void wifiHandleReboot() {
    server.send(200, "text/plain", "ok");
    delay(100);
    ESP.restart();
}

// ... setup wifi
void setupWifi() {
    wifiMgrExpose(&server);
#ifdef WIFI_SSID
    debugPrintln("setupWifi with hard coded credentials");
    setupWifi(WIFI_SSID, WIFI_PASSWORD, WIFI_HOST);
#else
    debugPrintln("setupWifi with portal");
    wifiMgrConfigureEEPROM(1, 4095);
    // eeprom config
    wifiMgrPortalSetup(false, "WiFiVitotrol-", "p0rtal123");
    debugPrintln("setupWifi with portal complete");
#endif
    setRescanInterval(-1);

    uint8_t p = 0;
    while (writableDataPoint[p] != nullptr) {
        server.on("/get" + String(writableDataPoint[p]->getName()), wifiHandleGetData);
        server.on("/set" + String(writableDataPoint[p]->getName()), HTTP_POST, wifiHandleSetData);
        server.on("/getSource" + String(writableDataPoint[p]->getName()), wifiHandleGetSource);
        debugPrint("dynamically created handler /get- and /set");
        debugPrintln(String(writableDataPoint[p]->getName()));
        p++;
    }

    server.on("/", wifiHandleGetOverview);
    server.on("/log", wifiHandleGetLog);
    server.on("/linkState", wifiHandleGetLinkState);
    server.on("/get", wifiHandleGetData);
    server.on("/set", HTTP_POST, wifiHandleSetData);
    server.on("/isConnected", wifiHandleIsConnected);
    server.on("/reboot", wifiHandleReboot);
    server.on("/registers", wifiHandleReadRegisters);
    server.on("/dataset", wifiHandleReadDataset);
    server.on("/datasets", wifiHandleReadDatasets);
    server.on("/requestDataset", wifiHandleRequestDataset);
    server.on("/getRequestDataset", wifiHandleGetRequestDataset);

#ifdef WIFI_SSID
    debugPrintln("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }
    if (!MDNS.begin(WIFI_HOST)) {
        debugPrintln("Error setting up MDNS responder!");
    }
    debugPrintln(WiFi.localIP());
    server.begin();
#endif
}

void wifiLoop() {
    server.handleClient();
}