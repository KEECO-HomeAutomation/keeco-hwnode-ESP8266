void startSoftAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  sprintf(AP_SSID, "KEECO_AP_%06X", ESP.getChipId());
  WiFi.softAP(AP_SSID, AP_Password);
#ifdef DEBUG
  Serial.println("Soft AP Running...");
#endif
}

void startWebserver() {

  //WiFi and HW NodeInfo related pages
  webserver.on("/", handleRoot);
  webserver.on("/wifisave", handleWifiSave);
  webserver.on("/info", handleNodeInfo);
  webserver.on("/uuid", handleUuidRequest);

  //FS Browser related pages
  webserver.on("/list", HTTP_GET, handleFileList);                                         //list directory
  webserver.on("/edit", HTTP_GET, []() {                                                   //load editor
    if (!handleFileRead("/edit.htm")) webserver.send(404, "text/plain", "FileNotFound");
  });
  webserver.on("/edit", HTTP_PUT, handleFileCreate);                                       //create file
  webserver.on("/edit", HTTP_DELETE, handleFileDelete);                                    //delete file
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  webserver.on("/edit", HTTP_POST, []() {
    webserver.send(200, "text/plain", "");
  }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  webserver.onNotFound([]() {
    if (!handleFileRead(webserver.uri()))
      webserver.send(404, "text/plain", "FileNotFound");
  });
  //Start the WebServer
  webserver.begin();
#ifdef DEBUG
  Serial.println("Webserver running...");
#endif
}

void start_mDNS() {
  sprintf(hostString, "KEECO_%06X", ESP.getChipId());
  if (!MDNS.begin(hostString)) {
#ifdef DEBUG
    Serial.println("ERROR: mDNS Responder could not start!");
#endif
  }
  else  {
#ifdef DEBUG
    Serial.println("mDNS Running, Hoststring with ChipID");
    Serial.println(hostString);
#endif
  }
}

void stopAPServices() {
  WiFi.mode(WIFI_STA);
}

void webserverInLoop() {
  webserver.handleClient();
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
