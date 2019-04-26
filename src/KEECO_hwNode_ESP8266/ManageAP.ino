void startSoftAP() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  sprintf(AP_SSID, "KEECO_AP_%06X", ESP.getChipId());
  WiFi.softAP(AP_SSID, AP_Password);
#ifdef DEBUG
  Serial.println("Soft AP Running...");
#endif
}

void stopAPServices() {
  WiFi.mode(WIFI_STA);
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
