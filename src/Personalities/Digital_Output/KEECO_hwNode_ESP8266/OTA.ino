void InitOTA() {
#ifdef OTA
    ArduinoOTA.setHostname(espConfig.hostString);
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

void OTAInLoop() {
#ifdef OTA
    ArduinoOTA.handle();
#endif
}
