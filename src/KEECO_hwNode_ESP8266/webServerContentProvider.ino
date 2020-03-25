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
      "</div><form method='POST' action='send_new_settings'>"
      "<h4>Provide new TLS Cert fingerprint:</h4>"
      "<input type='text' placeholder='TLS Fingerprint' name='fingerprint'/><br>"
      "<h4>Provide new UUID:</h4>"
      "<input type='text' placeholder='New UUID' name='uuid'/><br>"
      "<h4>Provide new MQTT server:</h4>"
      "<input type='text' placeholder='New MQTT Server' name='mqttserver'/><br>"
      "<h4>Provide new MQTT Username:</h4>"
      "<input type='text' placeholder='New MQTT Username' name='mqttusername'/><br>"
      "<h4>Provide new MQTT Password</h4>"
      "<input type='password' placeholder='New MQTT Password' name='mqttpassword'/><br>"
      "<input type='submit' value='Update Settings'/></form>"
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
  String wifiDatalist = "";
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
  wifiDatalist = "<datalist id='ssids'>";
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      webserver.sendContent("<div class=\"grid-item\">" + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : " * ") + " (" + WiFi.RSSI(i) + ")</div>");
      wifiDatalist = wifiDatalist + "<option>" + WiFi.SSID(i) + "</option>";
    }
  } else {
    webserver.sendContent(String() + "<div class=\"grid-item\">No WLAN found</td></div>");
    wifiDatalist = wifiDatalist + "<option>No networks found</option>";
  }
  wifiDatalist = wifiDatalist + "</datalist>";
  webserver.sendContent(
    "</div"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<input type='text' placeholder='SSID' list='ssids' name='n'/>" + wifiDatalist + 
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
  espConfig.updateConfigJSON();
  espConfig.saveConfig();
  if (strlen(espConfig.wifiSTA.ssid) > 0) {
    WiFi.disconnect();
    WiFi.begin ( espConfig.wifiSTA.ssid, espConfig.wifiSTA.password );
  }
}

void handleLoginAttempt() {
  char temp_password[36];
  webserver.arg("password").toCharArray(temp_password, 36);
  if (strcmp(temp_password, "12345678") == 0) {
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

void handleReceiveSettings() {                                
  char temp_string[64];
  if ( webserver.arg("mqttserver") != NULL) {
    webserver.arg("mqttserver").toCharArray(temp_string, 64);
    strcpy(espConfig.mqttServer, temp_string);
    Serial.println(espConfig.mqttServer);
  }
  if ( webserver.arg("mqttusername") != NULL) {
    webserver.arg("mqttusername").toCharArray(temp_string, 64);
    strcpy(espConfig.mqttUsername, temp_string);
        Serial.println(espConfig.mqttUsername);
  }
  if ( webserver.arg("mqttpassword") != NULL) {
    webserver.arg("mqttpassword").toCharArray(temp_string, 64);
    strcpy(espConfig.mqttPassword, temp_string);
        Serial.println(espConfig.mqttPassword);
  }
  if ( webserver.arg("uuid") != NULL) {
    webserver.arg("uuid").toCharArray(temp_string, 64);
    strcpy(espConfig.deviceUUID, temp_string);
        Serial.println(espConfig.deviceUUID);
  }
    if ( webserver.arg("fingerprint") != NULL) {
    webserver.arg("fingerprint").toCharArray(temp_string, 64);
    strcpy(espConfig.fingerprint, temp_string);
        Serial.println(espConfig.fingerprint);
  }
  espConfig.updateConfigJSON();
  espConfig.saveConfig();
  webserver.sendHeader("Location", "/", true);
  webserver.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webserver.sendHeader("Pragma", "no-cache");
  webserver.sendHeader("Expires", "-1");
  webserver.send ( 303, "text/plain", "/");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
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
