/* Load WLAN credentials from EEPROM */
void loadWifiCredentials() {

  char EOS[3];
  
  EEPROM.begin(512);
  EEPROM.get(0, WiFi_SSID);
  EEPROM.get(0+sizeof(WiFi_SSID), WiFi_Password);  
  EEPROM.get(0+sizeof(WiFi_SSID)+sizeof(WiFi_Password), EOS);
  EEPROM.end();
  
  if (String(EOS) != String("OK")) {
    WiFi_SSID[0] = 0;
    WiFi_Password[0] = 0;
  }
  #ifdef DEBUG 
    Serial.println("Credentials from EEPROM:");
    Serial.println(WiFi_SSID);
    Serial.println(WiFi_Password);
  #endif
}

/** Store WLAN credentials to EEPROM */
void storeWifiCredentials() {

  char EOS[3] = "OK";
  
  EEPROM.begin(512);
  EEPROM.put(0, WiFi_SSID);
  EEPROM.put(0+sizeof(WiFi_SSID), WiFi_Password);

  EEPROM.put(0+sizeof(WiFi_SSID)+sizeof(WiFi_Password), EOS);
  EEPROM.commit();
  EEPROM.end();

  #ifdef DEBUG 
    Serial.println("Credentials stored to EEPROM:");
    Serial.println(WiFi_SSID);
    Serial.println(WiFi_Password);
  #endif
  
}
