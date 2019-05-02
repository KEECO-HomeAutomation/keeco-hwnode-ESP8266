void OTA_Setup() {
#ifdef OTA
    ArduinoOTA.setHostname(hostString);
    ArduinoOTA.onStart([]() {
        Serial.println("Arduino OTA InProgress...");
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("Arduino OTA Ended");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.println("Arduino OTA Error..restarting...");
        (void)error;
        ESP.restart();
    });
    ArduinoOTA.begin();
    Serial.println("OTA Available");
#endif
}

void OTA_inLoop() {
#ifdef OTA
    ArduinoOTA.handle();
#endif
}
