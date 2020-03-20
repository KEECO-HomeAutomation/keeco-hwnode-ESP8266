void handleRoot() {
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if (espConfig.statuses.authenticated) {
    webserver.sendContent(
      "<html><head>" + css_style + "</head><body>"
      "<h1>KEECO HW Node " + String(espConfig.wifiAP.ssid) + "</h1>"
      "<h2>WiFi Configuration</h2>"
    );
    if (webserver.client().localIP() == apIP) {
      webserver.sendContent(String("<p>You are connected to this AP: ") + espConfig.wifiAP.ssid + "</p>");
    } else {
      webserver.sendContent(String("<p>You are connected to this WiFi network: ") + espConfig.wifiSTA.ssid + "</p>");
    }
    webserver.sendContent(
      "\r\n<br />"
      "<div class=\"grid-container\">"
    );
    webserver.sendContent("<div class=\"grid-item\">SoftAP configuration <br>");
    webserver.sendContent("SSID " + String(espConfig.wifiAP.ssid) + "<br>");
    webserver.sendContent("IP " + toStringIp(WiFi.softAPIP()) + "</div>");
    
    webserver.sendContent("<div class=\"grid-item\">WLAN configuration<br>");
    webserver.sendContent("SSID " + String(espConfig.wifiSTA.ssid) + "<br>");
    webserver.sendContent("IP " + toStringIp(WiFi.localIP()) + "</div></div>");
    webserver.sendContent(
      "</div><form method='POST' action='set_fingerprint'><h4>Provide new TLS Cert fingerprint:</h4>"
      "<input type='text' placeholder='TLS Fingerprint' name='fingerprint'/>"
      "<br /><input type='submit' value='Update Fingerprint'/></form>"
    );
    webserver.sendContent(
      "<form method='POST' action='set_uuid'><h4>Provide new UUID:</h4>"
      "<input type='text' placeholder='New UUID' name='uuid'/>"
      "<br /><input type='submit' value='Update UUID'/></form>"
    );
    webserver.sendContent(
      "<form method='POST' action='set_mqttsrv'><h4>Provide new MQTT server:</h4>"
      "<input type='text' placeholder='New MQTT server' name='mqtt'/>"
      "<br /><input type='submit' value='Update MQTT server'/></form>"
    );
    webserver.sendContent(
      "<br /><a href='/wifisettings'>WiFi Settings</a>"
      "<br /><a href='/info'>Display Node Info File</a>"
      "<br /><a href='/mdns'>Display MQTT service providers on this network</a>"
      "</body></html>"
    );
    webserver.client().stop(); // Stop is needed because we sent no content length
  }
  else {
    webserver.sendContent(
      "<html><head>" + css_style + "</head><body>"
      "<h1>KEECO HW Node " + String(espConfig.wifiAP.ssid) + "</h1>"
      "<h2>Login to KEECO</h2>"
      "<form method='POST' action='login'><h3>Enter password:</h3>"
      "<input type='password' placeholder='login password' name='password'/>"
      "<br /><input type='submit' value='Login'/></form>"
      );
  }
}
void handleWifiSettings() {
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.sendContent(
    "<html><head>" + css_style + "</head><body>"
    "<h1>WiFi Configuration</h1>"
  );
  if (webserver.client().localIP() == apIP) {
    webserver.sendContent(String("<h3>You are connected to this AP: ") + espConfig.wifiAP.ssid + "</h3>");
  } else {
    webserver.sendContent(String("<h3>You are connected to this WiFi network: ") + espConfig.wifiSTA.ssid + "</h3>");
  }
  webserver.sendContent(
      "\r\n<br />"
      "<div class=\"grid-container\">"
    );
    webserver.sendContent("<div class=\"grid-item\">SoftAP configuration <br>");
    webserver.sendContent("SSID " + String(espConfig.wifiAP.ssid) + "<br>");
    webserver.sendContent("IP " + toStringIp(WiFi.softAPIP()) + "</div>");
    
    webserver.sendContent("<div class=\"grid-item\">WLAN configuration<br>");
    webserver.sendContent("SSID " + String(espConfig.wifiSTA.ssid) + "<br>");
    webserver.sendContent("IP " + toStringIp(WiFi.localIP()) + "</div></div>");
  webserver.sendContent(
    "\r\n<br />"
    "<h3>WLAN list (refresh page to update list)</h3>"
    "<div class=\"grid-container\">"
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
      webserver.sendContent("<div class=\"grid-item\">SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " *") + " (" + WiFi.RSSI(i) + ")</div>");
    }
  } else {
    webserver.sendContent(String() + "<div class=\"grid-item\">No WLAN found</td></div>");
  }
  webserver.sendContent(
    "</div"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<input type='text' placeholder='SSID' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "<br /><a href='/'>Return to Main Page</a>"
    "</body></html>"
  );
  webserver.client().stop(); // Stop is needed because we sent no content length
}


/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() {
  webserver.arg("n").toCharArray(espConfig.wifiSTA.ssid, sizeof(espConfig.wifiSTA.ssid) - 1);
  webserver.arg("p").toCharArray(espConfig.wifiSTA.password, sizeof(espConfig.wifiSTA.password) - 1);
  webserver.sendHeader("Location", "/", true);
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.client().stop(); // Stop is needed because we sent no content length
  espConfig.storeWifiCredentials();
  if (strlen(espConfig.wifiSTA.ssid) > 0) {
    WiFi.disconnect();
    WiFi.begin ( espConfig.wifiSTA.ssid, espConfig.wifiSTA.password );
  }
}

void handleLoginAttempt() {
  char temp_password[36];
  webserver.arg("password").toCharArray(temp_password, 36);
  if (strcmp(temp_password,"12345678")==0) {
    espConfig.statuses.authenticated = true;
    espConfig.statuses.auth_timestamp = millis();
    loginWebserver();
  }
  webserver.sendHeader("Location", "/", true);
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.send ( 303, "text/plain", "/");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.client().stop(); // Stop is needed because we sent no content length
}

void handleNodeInfo() {
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.sendContent(espConfig.getContentSerd());
  webserver.client().stop(); // Stop is needed because we sent no content length
}

void handleGetSettings() {
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webserver.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webserver.sendContent(espConfig.getContentSerd());
  webserver.client().stop(); // Stop is needed because we sent no content length
}

void handleSetUUID() {                         // If a POST request is made to URI /login
  char temp_uuid[37];
  if ( ! webserver.hasArg("uuid") || webserver.arg("uuid") == NULL) { // If the POST request doesn't have username and password data
    webserver.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    Serial.println("post receieved");
  }
  else {
    webserver.arg("mqtt").toCharArray(temp_uuid, 64);
    strcpy(espConfig.deviceUUID, temp_uuid);
    espConfig.storeFingerprint();
    webserver.sendHeader("Location", "/", true);
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.client().stop(); // Stop is needed because we sent no content length
  }
}

void handleSetMqttSrv() {                         // If a POST request is made to URI /login
  char temp_mqttsrv[64];
  if ( ! webserver.hasArg("mqtt") || webserver.arg("mqtt") == NULL) { // If the POST request doesn't have username and password data
    webserver.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    Serial.println("post receieved");
  }
  else {
    webserver.arg("mqtt").toCharArray(temp_mqttsrv, 64);
    strcpy(espConfig.mqttServer, temp_mqttsrv);
    espConfig.storeFingerprint();
    webserver.sendHeader("Location", "/", true);
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.client().stop(); // Stop is needed because we sent no content length
  }
}
void handlePostFingerprint() {     // If a POST request is made to URI /login
  char temp_fingerprint[64];
  if ( ! webserver.hasArg("fingerprint") || webserver.arg("fingerprint") == NULL) { // If the POST request doesn't have username and password data
    webserver.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    Serial.println("HTTP POST - Invalid Request");
  }
  else {
    webserver.arg("fingerprint").toCharArray(temp_fingerprint, 64);
    strcpy(espConfig.fingerprint, temp_fingerprint);
    espConfig.storeFingerprint();
    webserver.sendHeader("Location", "/", true);
    webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webserver.sendHeader("Pragma", "no-cache");
    webserver.sendHeader("Expires", "-1");
    webserver.send ( 302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    webserver.client().stop(); // Stop is needed because we sent no content length
  }
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
  if (espConfig.statuses.mdnsRunning) {
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
