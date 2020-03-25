ConfigurationHandler::ConfigurationHandler() {
#ifdef DEBUG
  Serial.println("ConfigurationHandler object has been created");
#endif
  sprintf(hostString, "KEECO_%06X", ESP.getChipId());
  strcpy(wifiAP.password, "12345678");
  strcpy(fingerprint, "empty");
  statuses.mdnsRunning = false;
  statuses.wifiIsConnected = false;
  statuses.softApRunning = false;
  defaultJson =
    "{\"ssid\":\"defaultSSID\",\"password\":\"12345678\",\"mqttserver\":\"mlhass.local\",\"mqttusername\":\"NA\",\"mqttpassword\":\"NA\",\"uuid\":\"NA\",\"tls_fingerprint\":\"00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\"}";
  statuses.authenticated = false;
}
char* ConfigurationHandler::getContentSerd() {
  return tempContentSerd;
}
void ConfigurationHandler::initConfiguration() {
  if ( ! SPIFFS.exists("/config.txt")) {
    File cfgFile = SPIFFS.open("/config.txt", "w");
    Serial.println("Config file doesn't exists, creating initial JSON file");
    cfgFile.print(defaultJson);
    cfgFile.close();
  }
  File configFile = SPIFFS.open("/config.txt", "r");
  fileSize = configFile.size();
  if (fileSize > 320) {
    fileSize = 319;
  }
  configFile.readBytes(tempContentSerd, fileSize);
  configFile.close();
  strcpy(tempContentDeserd, tempContentSerd);
  configError = deserializeJson(configDoc, tempContentDeserd);
  if (configError) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(configError.c_str());
    return;
  }
  const char* ssid = configDoc["ssid"].as<char*>();
  strcpy(wifiSTA.ssid, ssid);
  const char* password = configDoc["password"].as<char*>();
  strcpy(wifiSTA.password, password);
  const char* mqttsrvtemp = configDoc["mqttserver"].as<char*>();
  strcpy(mqttServer, mqttsrvtemp);
  const char* mqttuser = configDoc["mqttusername"].as<char*>();
  strcpy(mqttUsername, mqttuser);
  const char* mqttpasstemp = configDoc["mqttpassword"].as<char*>();
  strcpy(mqttPassword, mqttpasstemp);
  const char* temp_fingerprint = configDoc["tls_fingerprint"].as<char*>();
  strcpy(fingerprint, temp_fingerprint);

  const char* devUUID = configDoc["uuid"].as<char*>();
  if (strcmp(devUUID, "NA") == 0) {
    Serial.println("New UUID has been generated.");
    ESP8266TrueRandom.uuid(uuidNumber);
    uuidStr = ESP8266TrueRandom.uuidToString(uuidNumber);
    uuidStr.toCharArray(deviceUUID, 37);
    configDoc["uuid"] = deviceUUID;
    saveConfig();
  }
  else {
    strcpy(deviceUUID, devUUID);
  }
#ifdef DEBUG
  Serial.println("Config file has been successfully opened");
  Serial.println("Config file content:");
  Serial.println(tempContentSerd);
#endif
}

void ConfigurationHandler::updateConfigJSON() {
  configDoc.clear();
  configDoc["ssid"] = wifiSTA.ssid;
  configDoc["password"] = wifiSTA.password;
  configDoc["tls_fingerprint"] = fingerprint;
  configDoc["uuid"] = deviceUUID;
  configDoc["mqttserver"] = mqttServer;
  configDoc["mqttusername"] = mqttUsername;
  configDoc["mqttpassword"] =  mqttPassword;
  serializeJson(configDoc, tempContentSerd);
#ifdef DEBUG
  Serial.println("ConfigJSON has been updated");
#endif
}

void ConfigurationHandler::saveConfig() {
  File cfgFile = SPIFFS.open("/config.txt", "w");
  Serial.println("Saving config data");
  //serializeJson(configDoc, tempContentSerd);
  cfgFile.print(tempContentSerd);
  cfgFile.close();
#ifdef DEBUG
  Serial.println("Configuration file has been updated updated");
  Serial.println(tempContentSerd);
#endif
}

void ConfigurationHandler::serialCmdCheckInLoop() {
  if (Serial.available() > 0) {
    Serial.readBytesUntil('\r', serialData, 80);
    parseJsonPacket();
  }
}

bool ConfigurationHandler::parseJsonPacket() {
  error = deserializeJson(configUartDoc, serialData);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }
  const char* command = configUartDoc["command"].as<char*>();
  if (strcmp(command, "wifi") == 0) {
    strcpy(wifiSTA.ssid, configUartDoc["ssid"]);
    strcpy(wifiSTA.password, configUartDoc["password"]);
  }
  if (strcmp(command, "mqtt") == 0) {
    strcpy(mqttServer, configUartDoc["server"]);
  }
  if (strcmp(command, "fingerprint") == 0) {
    strcpy(fingerprint, configUartDoc["fingerprint"]);
  }
  if (strcmp(command, "save") == 0) {
    updateConfigJSON();
    saveConfig();
  }
  if (strcmp(command, "reset") == 0) {
    Serial.println("Resetting device...");
    ESP.restart();
  }
  if (strcmp(command, "read") == 0) {
    Serial.println(tempContentSerd);
  }
  if (strcmp(command, "help") == 0) {
    Serial.println("Available commands:");
    Serial.println("{Set new SSID and Password:");
    Serial.println("{'command':'wifi, 'ssid':'___', 'password':'____'}");
    Serial.println("Set new MQTT Server:");
    Serial.println("{'command':'mqtt', 'server':'___'}");
    Serial.println("Set new TLS fingerprint:");
    Serial.println("{'command':'fingerprint', 'fingerprint':'___'}");
    Serial.println("Save current settings into SPIFFS");
    Serial.println("{'command':'save'}");
    Serial.println("Reset device");
    Serial.println("{'command':'reset'}");
    Serial.println("Read settings JSON string");
    Serial.println("{'command':'read'}");
  }
  return true;
}
