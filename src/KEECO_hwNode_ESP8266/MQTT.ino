PubSubClient client(wifiClient);
#define MQTT_CONN_RETRY_WAIT 5000

long mqttLastConnAttempt = 0;

char *mqttSubTopic[] = {
    "SubTopic1",
    "SubTopic2"
};

int mqttSubTopicCount = 2;

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
    if (client.connect(AP_SSID, contentOfInfoTxt, "password")) {
        //if (client.connect(AP_SSID, "development", "development")) {
        for (int i = 0; i < mqttSubTopicCount ; i++ ) {
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
