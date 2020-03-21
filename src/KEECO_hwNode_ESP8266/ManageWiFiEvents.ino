void WiFiEvent(WiFiEvent_t event) {
#ifdef DEBUG
    Serial.printf("[WiFi-event] event: %d\n", event);
#endif
    switch (event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
#ifdef DEBUG
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
#endif
            if (espConfig.statuses.softApRunning) {
                stopAPServices();
            }
            espConfig.statuses.wifiIsConnected = true;
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            if (espConfig.statuses.wifiIsConnected) {
                if (!(espConfig.statuses.softApRunning)) {
                    startAPServices();
                    Serial.println("AP Services Started (192.168.4.1)");
                }
            }
            espConfig.statuses.wifiIsConnected = false;
#ifdef DEBUG
            Serial.println("WiFi lost connection");
#endif
            break;
    }
}
