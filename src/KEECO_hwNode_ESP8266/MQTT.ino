PubSubClient client(wifiClient);
#define MQTT_CONN_RETRY_WAIT 5000


long mqttLastConnAttempt = 0;
char *mqttSubTopic = "SubscribedTopic";
char *mqttPubTopic = "PublishTopic";

void mqttConectionSetup() {
  client.setServer(mqttServer, 1883);
  client.setCallback(mqttSubCallback);
}

void mqttSubCallback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

boolean mqttReconnect() {
  if (client.connect(deviceUUID, contentOfInfoTxt, NULL)) {
    client.publish(mqttPubTopic, "ONLINE");
    client.subscribe(mqttSubTopic);
#ifdef DBEUG
    Serial.println("Connected to MQTT Server");
#endif
  }
  return client.connected();
}

void mqttInLoop() {
  if (!client.connected()) {
#ifdef DEBUG
    Serial.println("Still no connection to MQTT Server");
#endif
    long now = millis();
    if (now - mqttLastConnAttempt > MQTT_CONN_RETRY_WAIT) {
      mqttLastConnAttempt = now;
      if (mqttReconnect()) {
        mqttLastConnAttempt = 0;
      }
    }
  }
  else {
    client.loop();
  }
}
