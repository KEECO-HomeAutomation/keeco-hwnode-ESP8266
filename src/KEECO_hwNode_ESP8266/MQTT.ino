WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);
#define MQTT_CONN_RETRY_WAIT 5000


long mqttLastConnAttempt = 0;

/*
   We use TLS certifications to encrypt the communication.
   WiFiClientSecure defines that the communication happens with TLS
   The fingerprint can be extracted on your computer from the cert file. We only use the fingerprint and not the whole cert not to take too much memory for this.
   Please note that you need to call the .verfiy function to actually check the fingerprint.
   It is important to understand that with this TLS method we achieve two things:
      1.) Integrity   - we make sure we connect to the right server
      2.) Encryption  - the data sent over the connection is encrypted (as defined in the cert file)
   This is a great tutorial to create your own TLS cert http://www.steves-internet-guide.com/mosquitto-tls/
*/

void initMqtt() {
  wifiClient.setFingerprint(espConfig.fingerprint);
  if (!wifiClient.connect(espConfig.mqttServer, 8883)) {
#ifdef DEBUG
    Serial.println("Connection to mqtts open port not successful");
#endif
  }
  if (!wifiClient.verify(espConfig.fingerprint, espConfig.mqttServer)) {
#ifdef DEBUG
    Serial.println("Fingerprint does not match");
#endif
  }
  client.setServer(espConfig.mqttServer, 8883);
  client.setCallback(mqttSubCallback);

}

void mqttSubCallback(char* topic, byte* payload, unsigned int length) {
  mqttReceivedCallback(topic, payload, length);
}

boolean mqttReconnect() {
  char temp_topic[128];
  if (client.connect(espConfig.wifiAP.ssid, espConfig.mqttUsername, espConfig.mqttPassword)) {
    for (int i = 0; i < espConfig.mqttSubTopicCount ; i++ ) {
      appendSubtopicToNew(espConfig.mqttSubTopic[i], temp_topic);
      Serial.println("Subscribed to MQTT Topic: ");
      Serial.println(temp_topic);
      client.subscribe(temp_topic);
    }
    announceNodeState();
#ifdef DEBUG
    Serial.println("Connected to MQTT Server");
#endif
  }
  return client.connected();
}

void mqttInLoop() {
  if (!client.connected()) {
    long now = millis();
    if (now - mqttLastConnAttempt > MQTT_CONN_RETRY_WAIT) {
      mqttLastConnAttempt = now;
      if (mqttReconnect()) {
        Serial.println("MQTT Reconnect Successful.");
      }
      else {
#ifdef DEBUG
        Serial.println("Still no connection to MQTT Server");
        Serial.println(client.state());
#endif
      }
    }
  }
  client.loop();
}


void appendSubtopic(char *inputTopic ) {
  char temp_topic[128] = " ";
  strcpy(temp_topic, espConfig.deviceUUID);
  strcat(temp_topic, inputTopic);
  strcpy(inputTopic, temp_topic);
}
void appendSubtopicToNew(char *inputTopic, char *outputTopic) {
  char temp_topic[128] = " ";
  strcpy(temp_topic, espConfig.deviceUUID);
  strcat(temp_topic, inputTopic);
  strcpy(outputTopic, temp_topic);
}

void mqttPublish(char *pub_subtopic, char *temp_topic, byte *mqtt_buffer, int byte_length) {
  appendSubtopicToNew(pub_subtopic, temp_topic);
  client.publish(temp_topic, mqtt_buffer, byte_length);
}
