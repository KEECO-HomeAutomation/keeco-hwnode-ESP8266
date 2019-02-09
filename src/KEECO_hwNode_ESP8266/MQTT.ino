WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);


bool connectToMqttBroker() {
    if (!mqttClient.connect(broker, 1883)) {
      #ifdef DEBUG
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
      #endif
      return false;
    }
    else {
      #ifdef DEBUG
        Serial.print("Connected to MQTT broker");
      #endif
    }
      return true;
}

void initializeMqttSession() {
  mqttClient.onMessage(onMqttMessage);     // set the message receive callback
  mqttClient.subscribe(topic);             // subscribe to a topic
}

void mqttInLoop() {
    mqttClient.poll();  
}


void onMqttMessage(int messageSize) {     //MQTT receive callback
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  while (mqttClient.available()) {        // use the Stream interface to print the contents
    Serial.print((char)mqttClient.read());
  }
}

void mqttSendMessage() {
    mqttClient.beginMessage(topic);
    mqttClient.print("hello ");
    mqttClient.endMessage();
}
