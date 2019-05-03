void wifiInitOnBoot() {
    int wifiConnWait = 0;
    sprintf(AP_SSID, "KEECO_AP_%06X", ESP.getChipId());
    WiFi.mode(WIFI_STA);
    WiFi.onEvent(WiFiEvent);
    WiFi.setAutoReconnect(true);
    WiFi.hostname("KKECO_HW_Node");
    WiFi.begin(WiFi_SSID, WiFi_Password);
#ifdef DEBUG
    Serial.println("Connecting to WiFi...");
#endif
    while ((WiFi.status() != WL_CONNECTED) & (wifiConnWait < 60)) {
        delay(250);
        wifiConnWait++;
    }
    if (wifiConnWait < 60) {
#ifdef DEBUG
        Serial.println("");
        Serial.print("Connected to Wifi");
        Serial.println(WiFi_SSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
#endif
        wifiIsConnected = true;
    }
    else {
        startAPServices();
    }
    start_mDNS();
}


void startAPServices() {
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(AP_SSID, AP_Password);
#ifdef DEBUG
    Serial.println("Soft AP Running...");
#endif
    if (mdnsRunning) {
        MDNS.notifyAPChange();
    }
    softAPRunning = true;
}

void stopAPServices() {
    WiFi.softAPdisconnect(true);
    if (mdnsRunning) {
        MDNS.notifyAPChange();
    }
    softAPRunning = false;
#ifdef DEBUG
    Serial.println("Soft AP is Stopped");
#endif
}

/** Is this an IP? */
boolean isIp(String str) {
    for (int i = 0; i < str.length(); i++) {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9')) {
            return false;
        }
    }
    return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
    String res = "";
    for (int i = 0; i < 3; i++) {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}
