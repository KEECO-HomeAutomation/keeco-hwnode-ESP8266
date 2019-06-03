PubSubClient client(wifiClient);
#define MQTT_CONN_RETRY_WAIT 5000

long mqttLastConnAttempt = 0;

char *mqttSubTopic[] = {
//@mqttSubTopics@
};

int mqttSubTopicCount = 0;		//Do not modify this when using the build tool! This line is replaced in the automatic code generator tool!

void mqttConectionSetup() {
    query_mDNS();
    client.setServer(mqttServer, 1883);
    client.setCallback(mqttSubCallback);
}

void mqttSubCallback(char* topic, byte* payload, unsigned int length) {
#ifdef DEBUG
    Serial.print("Received topic:");
    Serial.println(topic);
    Serial.print("Received Payload: ");
    for (int i = 0; i < length; i++) {
        Serial.print(payload[i]);
    }
#endif
    setOutputs(topic, payload, length);
}

boolean mqttReconnect() {
  String temp_topic;
  
    if (client.connect(AP_SSID, contentOfInfoTxt, "password")) {
        for (int i = 0; i < mqttSubTopicCount ; i++ ) {
          temp_topic = String(mqttSubTopic[i]);
          temp_topic.replace("UUID_PLACEHOLDER", deviceUUID); 
          temp_topic.toCharArray(mqttSubTopic[i], 1024);
            Serial.println(mqttSubTopic[i]);
            client.subscribe(mqttSubTopic[i]);
        }
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
                Serial.println("mqttReconnect tortent");
                //mqttLastConnAttempt = 0;
            }
            else {
#ifdef DEBUG
                Serial.println("Still no connection to MQTT Server");
                Serial.println(client.state());
#endif
            }
        }
    }
    //  else {
    client.loop();
    //  }
}
