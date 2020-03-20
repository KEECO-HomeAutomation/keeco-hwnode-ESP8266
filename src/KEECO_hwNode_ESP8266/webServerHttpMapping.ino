#define AUTO_LOGOUT_TIMEOUT 300000

void initWebserver() {
  webserver.on("/", handleRoot);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);
  webserver.begin();
#ifdef DEBUG
  Serial.println("Webserver initialized...");
#endif
}

void logoutWebserver() {
  webserver.stop();
  webserver.on("/", handleRoot);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);
  webserver.begin();
#ifdef DEBUG
  Serial.println("Webserver configured to user logged out");
#endif
}

void loginWebserver() {
  webserver.stop();
  webserver.on("/", handleRoot);
  webserver.on("/wifisettings", handleWifiSettings);
  webserver.on("/wifisave", handleWifiSave);
  webserver.on("/info", handleNodeInfo);
  webserver.on("/mdns", mdnsQueryHandler);
  webserver.on("/settings", HTTP_GET, handleGetSettings);
  webserver.on("/set_uuid", HTTP_POST, handleSetUUID);
  webserver.on("/set_mqttsrv", HTTP_POST, handleSetMqttSrv);
  webserver.on("/set_fingerprint", HTTP_POST, handlePostFingerprint);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);
  webserver.begin();
#ifdef DEBUG
  Serial.println("Webserver configured to user logged in");
#endif
}
/*
  void loginWebserver() {

  //WiFi and HW NodeInfo related pages
  webserver.on("/", handleRoot);
  webserver.on("/wifisettings", handleWifiSettings);
  webserver.on("/wifisave", handleWifiSave);
  webserver.on("/info", handleNodeInfo);
  webserver.on("/uuid", handleUuidRequest);
  webserver.on("/mdns", mdnsQueryHandler);

  webserver.on("/settings", HTTP_GET, handleGetSettings);
  webserver.on("/set_uuid", HTTP_POST, handleSetUUID);
  webserver.on("/set_mqttsrv", HTTP_POST, handleSetMqttSrv);
  webserver.on("/set_fingerprint", HTTP_POST, handlePostFingerprint);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);

  webserver.begin();
  #ifdef DEBUG
  Serial.println("Webserver running...");
  #endif
  }
*/

void webserverInLoop() {
  webserver.handleClient();
  if (espConfig.statuses.authenticated) {
    if (espConfig.statuses.auth_timestamp + AUTO_LOGOUT_TIMEOUT < millis()) {
      espConfig.statuses.authenticated = false;
      logoutWebserver();
    }
  }
}
