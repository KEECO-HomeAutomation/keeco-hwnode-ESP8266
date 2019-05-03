void handleRoot() {
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.sendContent(
        "<html><head></head><body>"
        "<h1>KEECO HW Node " + String(AP_SSID) + "</h1>"
        "<h2>WiFi Configuration</h2>"
    );
    if (webserver.client().localIP() == apIP) {
        webserver.sendContent(String("<p>You are connected to this AP: ") + AP_SSID + "</p>");
    } else {
        webserver.sendContent(String("<p>You are connected to this WiFi network: ") + WiFi_SSID + "</p>");
    }
    webserver.sendContent(
        "\r\n<br />"
        "<table><tr><th align='left'>SoftAP configuration</th></tr>"
    );
    webserver.sendContent(String() + "<tr><td>SSID " + String(AP_SSID) + "</td></tr>");
    webserver.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
    webserver.sendContent(
        "</table>"
        "\r\n<br />"
        "<table><tr><th align='left'>WLAN settings</th></tr>"
    );
    webserver.sendContent(String() + "<tr><td>SSID " + String(WiFi_SSID) + "</td></tr>");
    webserver.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
    webserver.sendContent(
        "</table>"
        "\r\n<br />"
        "<table><tr><th align='left'>WLAN lista (refresh page to update list)</th></tr>"
    );
#ifdef DEBUG
    Serial.println("Scan Start");
#endif
    int n = WiFi.scanNetworks();
#ifdef DEBUG
    Serial.println("Scan Done");
#endif
    if (n > 0) {
        for (int i = 0; i < n; i++) {
            webserver.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
        }
    } else {
        webserver.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
    }
    webserver.sendContent(
        "</table>"
        "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network::</h4>"
        "<input type='text' placeholder='SSID' name='n'/>"
        "<br /><input type='password' placeholder='password' name='p'/>"
        "<br /><input type='submit' value='Connect/Disconnect'/></form>"
        "<br /><a href='/info'>Display Node Info File</a>"
        "<br /><a href='/uuid'>Display Node UUID</a>"
        "<br /><a href='/edit'>Display Node File Manager</a>"
        "<br /><a href='/mdns'>Display results of an mDNS Query</a>"
        "</body></html>"
    );
    webserver.client().stop(); // Stop is needed because we sent no content length
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
    webserver.arg("n").toCharArray(WiFi_SSID, sizeof(WiFi_SSID) - 1);
    webserver.arg("p").toCharArray(WiFi_Password, sizeof(WiFi_Password) - 1);
    webserver.sendHeader("Location", "/", true);
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.client().stop(); // Stop is needed because we sent no content length
    storeWifiCredentials();
    if (strlen(WiFi_SSID) > 0) {
        WiFi.disconnect();
        WiFi.begin ( WiFi_SSID, WiFi_Password );
    }
}

void handleNodeInfo() {
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.sendContent(contentOfInfoTxt);
    webserver.client().stop(); // Stop is needed because we sent no content length
}

void handleUuidRequest() {
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.sendContent(deviceUUID);
    webserver.client().stop(); // Stop is needed because we sent no content length
}

void mdnsQueryHandler() {
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
    webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.sendContent(
        "<html><head></head><body>"
        "<h1>MQTT mDNS Query</h1>"
    );
    if (mdnsRunning) {
        int n = MDNS.queryService("mqtt", "tcp");
        if (n > 0) {
            for (int i = 0; i < n; i++) {
                webserver.sendContent(String() + "\r\n<tr><td>" + MDNS.hostname(i) + " @ " + toStringIp(MDNS.IP(i)) + ":" + String(MDNS.port(i)) + "</td></tr>");
            }
        } else {
            webserver.sendContent(String() + "<tr><td>No MQTT Service Provider was found</td></tr>");
        }
    }
    else {
        webserver.sendContent(String() + "<tr><td>Error: mDNS Service could not start on this node</td></tr>");
    }
    webserver.sendContent("</body></html>");
    webserver.client().stop(); // Stop is needed because we sent no content length
}
