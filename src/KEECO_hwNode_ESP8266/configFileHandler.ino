ConfigurationHandler::ConfigurationHandler() {
#ifdef DEBUG
  Serial.println("ConfigurationHandler object has been created");
#endif
  sprintf(hostString, "KEECO_%06X", ESP.getChipId());
  strcpy(wifiAP.password, "12345678");
  mqttServer = "empty";
  strcpy(fingerprint, "empty");
  statuses.mdnsRunning = false;
  statuses.wifiIsConnected = false;
  statuses.softApRunning = false;
  defaultJson =
    "{\"ssid\":\"defaultSSID\",\"password\":\"12345678\",\"mqtt\":\"mlhass.local\",\"uuid\":\"NA\",\"tls_fingerprint\":\"00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\"}";
  statuses.authenticated = false;
}
char* ConfigurationHandler::getContentSerd() {
  return tempContentSerd;
}
void ConfigurationHandler::initConfiguration() {
  if (SPIFFS.exists("/config.txt")) {
    Serial.println("Config file exists");
    File configFile = SPIFFS.open("/config.txt", "r");
    fileSize = configFile.size();
    if (fileSize > 255) {
      fileSize = 254;
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
    const char* mqtt = configDoc["mqtt"].as<char*>();
    strcpy(mqttServer, mqtt);
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
  else {
#ifdef DEBUG
    Serial.println("Config File not found");
#endif
    deserializeJson(configDoc, defaultJson);
    ESP8266TrueRandom.uuid(uuidNumber);
    uuidStr = ESP8266TrueRandom.uuidToString(uuidNumber);
    uuidStr.toCharArray(deviceUUID, 37);
    configDoc["uuid"] = deviceUUID;
    saveConfig();
#ifdef DEBUG
    Serial.println("Config file has been successfully created");
    Serial.println("Config file content:");
    Serial.println(tempContentSerd);
#endif
  }
}
void ConfigurationHandler::storeWifiCredentials() {
  configDoc["ssid"] = wifiSTA.ssid;
  configDoc["password"] = wifiSTA.password;
  saveConfig();
#ifdef DEBUG
  Serial.println("WiFI credentials were updated");
  Serial.println(tempContentSerd);
#endif
}
void ConfigurationHandler::storeFingerprint() {
  configDoc["tls_fingerprint"] = fingerprint;
  saveConfig();
#ifdef DEBUG
  Serial.println("TLS Fingerprint was updated");
  Serial.println(tempContentSerd);
#endif
}
void ConfigurationHandler::storeUUID() {
  configDoc["uuid"] = deviceUUID;
  saveConfig();
#ifdef DEBUG
  Serial.println("UUID was updated");
  Serial.println(tempContentSerd);
#endif
}
void ConfigurationHandler::storeMqttSrv() {
  configDoc["mqtt"] = mqttServer;
  saveConfig();
#ifdef DEBUG
  Serial.println("MQTT Server was updated");
  Serial.println(tempContentSerd);
#endif
}

void ConfigurationHandler::saveConfig() {
  File cfgFile = SPIFFS.open("/config.txt", "w");
  Serial.println("Saving config data:");
  Serial.println(tempContentSerd);
  serializeJson(configDoc, tempContentSerd);
  cfgFile.print(tempContentSerd);
  cfgFile.close();
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
    configUartDoc["ssid"] = wifiSTA.ssid;
    configUartDoc["password"] = wifiSTA.password;
  }
  if (strcmp(command, "mqtt") == 0) {
    strcpy(mqttServer, configUartDoc["server"]);
    configUartDoc["mqtt"] = mqttServer;
  }
  if (strcmp(command, "fingerprint") == 0) {
    strcpy(fingerprint, configUartDoc["fingerprint"]);
    configUartDoc["fingerprint"] = fingerprint;
  }
  if (strcmp(command, "save") == 0) {
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
    Serial.println("wifi + {'ssid':'___','password':'____'} ->  set new SSID and Password");
    Serial.println("mqtt + {'server':'___'}                 ->  set new MQTT Server");
    Serial.println("fingerprint + {'fingerprint':'___'}     ->  set new TLS fingerprint");
    Serial.println("save                                    ->  save current settings into SPIFFS");
    Serial.println("reset                                   ->  reset device");
    Serial.println("read                                    ->  read settings JSON string");
  }
  return true;
}
