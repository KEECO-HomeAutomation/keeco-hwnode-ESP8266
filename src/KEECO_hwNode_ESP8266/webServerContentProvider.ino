void handleRoot() {
  String responseString = "";
  if (espConfig.statuses.authenticated) {
    responseString = String("<html><head>");
#ifdef CSS
    responseString += css_style;
#endif
    responseString += String(
                        "</head><body>"
                        "<h1>KEECO HW Node ");
    responseString += String(espConfig.wifiAP.ssid);
    responseString += String(
                        "</h1>"
                        "<h2>WiFi Status</h2>");
    if (webserver.client().localIP() == apIP) {
      responseString +=
        String("<p>KEECO provides AP: ") + espConfig.wifiAP.ssid + String("</p>");
    } else {
      responseString +=
        String("<p>You are connected to this WiFi network: ") + espConfig.wifiSTA.ssid + String("</p>");
    }
    responseString += String(
                        "<br>"
                        "<div class=\"grid-container\">"
                        "<div class=\"grid-item\">SoftAP configuration <br>"
                        "SSID ");
    responseString += String(espConfig.wifiAP.ssid);
    responseString += String("<br> IP ");
    responseString += toStringIp(WiFi.softAPIP());
    responseString += String("</div>"
                             "<div class=\"grid-item\">WLAN configuration<br>"
                             "SSID ");
    responseString += String(espConfig.wifiSTA.ssid);
    responseString += String("<br> IP ");
    responseString += toStringIp(WiFi.localIP());
    responseString += String(
                        "</div></div>"
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
                        "<br><a href='/wifisettings'>WiFi Settings</a>"
                        "<br><a href='/info'>Display Node Info File</a>"
                        "<br><a href='/mdns'>Display MQTT service providers on this network</a>"
                        "</body></html>");
  }
  else {
    responseString = String("<html><head>");
#ifdef CSS
    responseString += css_style;
#endif
    responseString += String(
                        "</head><body>"
                        "<h1>KEECO HW Node ");
    responseString += String(espConfig.wifiAP.ssid);
    responseString += String(
                        "</h1>"
                        "<h2>Login to KEECO</h2>"
                        "<form method='POST' action='login'><h3>Enter password:</h3>"
                        "<input type='password' placeholder='login password' name='password'/>"
                        "<br><input type='submit' value='Login'/></form></body></html>");
  }
  webserver.send(200, "text/html", responseString); // Empty content inhibits Content-length header so we have to close the socket ourselves.
}
void handleWifiSettings() {
  String wifiDatalist = "";
  String responseString = "";
  responseString = String("<html><head>");
#ifdef CSS
  responseString += css_style;
#endif
  responseString += String("</head><body><h1>WiFi Configuration</h1>");
  if (webserver.client().localIP() == apIP) {
    responseString += String("<h3>You are connected to this AP: ") + espConfig.wifiAP.ssid + String("</h3>");
  } else {
    responseString += String("<h3>You are connected to this WiFi network: ") + espConfig.wifiSTA.ssid + String("</h3>");
  }
  responseString += String("<br>"
                           "<div class=\"grid-container\">"
                           "<div class=\"grid-item\">SoftAP configuration <br>"
                           "SSID ");
  responseString += String(espConfig.wifiAP.ssid);
  responseString += String("<br> IP ");
  responseString += toStringIp(WiFi.softAPIP());
  responseString += String("</div>"
                           "<div class=\"grid-item\">WLAN configuration<br>"
                           "SSID ");
  responseString += String(espConfig.wifiSTA.ssid);
  responseString += String("<br> IP ");
  responseString += toStringIp(WiFi.localIP());
  responseString += String("</div></div><br>");
#ifdef DEBUG
  Serial.println("Scan Start");
#endif
  int n = WiFi.scanNetworks();
#ifdef DEBUG
  Serial.println("Scan Done");
#endif
  wifiDatalist = String("<datalist id='ssids'>");
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      wifiDatalist += String("<option>");
      wifiDatalist += String(WiFi.SSID(i));
      wifiDatalist += String("</option>");
    }
  } else {
    wifiDatalist += String("<option>No networks found</option>");
  }
  wifiDatalist += String("</datalist>");
  responseString += String(
                      "<br><form method='POST' action='wifisave'><h4>Connect to network (refresh page to update list):</h4>"
                      "<input type='text' placeholder='SSID' list='ssids' name='n'/>");
  responseString += wifiDatalist;
  responseString += String(
                      "<br><input type='password' placeholder='password' name='p'/>"
                      "<br><input type='submit' value='Connect/Disconnect'/></form>"
                      "<br><a href='/'>Return to Main Page</a>"
                      "</body></html>");
  webserver.send(200, "text/html", responseString);  // Stop is needed because we sent no content length
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
  //webserver.client().stop(); // Stop is needed because we sent no content length
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
}

void handleNodeInfo() {
  webserver.send(200, "text/html", espConfig.getContentSerd()); // Empty content inhibits Content-length header so we have to close the socket ourselves.
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
  webserver.send ( 303, "text/plain", "/");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
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
        webserver.sendContent("\r\n<tr><td>" + MDNS.hostname(i) + " @ " + toStringIp(MDNS.IP(i)) + ":" + String(MDNS.port(i)) + "</td></tr>");
      }
    } else {
      webserver.sendContent("<tr><td>No MQTT Service Provider was found</td></tr>");
    }
  }
  else {
    webserver.sendContent(String() + "<tr><td>Error: mDNS Service could not start on this node</td></tr>");
  }
  webserver.sendContent("</body></html>");
  webserver.client().stop(); // Stop is needed because we sent no content length
}
