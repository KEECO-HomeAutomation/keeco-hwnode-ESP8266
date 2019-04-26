void WiFiEvent(WiFiEvent_t event) {
#ifdef DEBUG
  Serial.printf("[WiFi-event] event: %d\n", event);
#endif
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
      stopAPServices();
#ifdef DEBUG
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println("SoftAP stopped - remove device from current network to restart");
#endif
      wifiIsConnected = true;
      start_mDNS(); //start MDNS service
      mdnsQuery();
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      wifiIsConnected = false;
      startSoftAP();
#ifdef DEBUG
      Serial.println("WiFi lost connection");
      Serial.println("SoftAP has been restarted");
#endif
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
#ifdef DEBUG
      Serial.println("ESP8266 was scanned by an other device and it replied");
#endif
      break;
  }
}
