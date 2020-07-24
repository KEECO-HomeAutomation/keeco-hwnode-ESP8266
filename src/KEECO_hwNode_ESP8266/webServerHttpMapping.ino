#define AUTO_LOGOUT_TIMEOUT 300000

void initWebserver() {
  webserver.on("/", handleRoot);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);
  httpUpdater.setup(&webserver, update_path, update_username, update_password);
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
  webserver.on("/send_new_settings", HTTP_POST, handleReceiveSettings);
  webserver.on("/login", HTTP_POST, handleLoginAttempt);
  httpUpdater.setup(&webserver, update_path, update_username, update_password);
  webserver.begin();
#ifdef DEBUG
  Serial.println("Webserver configured to user logged in");
#endif
}

void webserverInLoop() {
  webserver.handleClient();
  if (espConfig.statuses.authenticated) {
    if (millis() - espConfig.statuses.auth_timestamp > AUTO_LOGOUT_TIMEOUT ) {
      espConfig.statuses.authenticated = false;
      logoutWebserver();
    }
  }
}
