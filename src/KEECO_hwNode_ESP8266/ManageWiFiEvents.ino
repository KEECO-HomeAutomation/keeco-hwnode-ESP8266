void WiFiEvent(WiFiEvent_t event) {
#ifdef DEBUG
  Serial.printf("[WiFi-event] event: %d\n", event);
#endif
  switch (event) {
    case WIFI_EVENT_STAMODE_GOT_IP:
#ifdef DEBUG
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
#endif
      wifiIsConnected = true;
      break;
    case WIFI_EVENT_STAMODE_DISCONNECTED:
      wifiIsConnected = false;
#ifdef DEBUG
      Serial.println("WiFi lost connection");
#endif
      break;
    case WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED:
#ifdef DEBUG
      Serial.println("ESP8266 was scanned by an other device and it replied");
#endif
      break;
  }
}
