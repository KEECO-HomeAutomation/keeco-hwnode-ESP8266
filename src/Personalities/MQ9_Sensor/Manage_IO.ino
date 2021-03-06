/*
   The code below is divided into 4 main segments:
     initIO()                   -   This code is called during device init. Place your init code here.
     IOprocessInLoop()          -   This code is called with every iteration in the main loop/
     timerCallback()            -   This code is called every N seconds (5 by default) using the timer
     mqttReceivedCallback(char* topic, byte* payload, unsigned int length)    -    This code is called everytime a subscribed MQTT topic receives a new message

   These additional helpers are available for you:
     CharToByte(char* chars, byte* bytes, unsigned int count)   -   Helper function for MQTT publish - publish requires byte array
     mqtt_send_buffer[64]                                       -   To temporarly store the message to be published
     announceNodeState()                                        -   Called during MQTT connection init. Can be called standalone at other times as well.
     appendSubtopic(char *topic)                                -   Appends the given topic to the UUID and stores the result in the topic. So all topics look like this: UUID/<status_topic>. Important to call once per topic only!
*/

/*
   Place your Arduino Shield specific #includes here
*/


/*
   When your device connects to an MQTT broker it announces its state in the UUID/<status_topic> topic
   Place your additional variable inits here
*/

byte mqtt_send_buffer[64];
char status_topic[128] = "/state";
char status_text[128] = "Device Online";

char sensor_topic[80] = "/sensor_input";
char sensor_string[64];
int sensorPin = A0;
int sensorValue = 0;

int relay_state = 1;
int sm_case = 1;
int relay_pin = 14;
unsigned long timestamp = 0;
unsigned long startup = 0;
int warmed_up = 0;
long average_off = 0;
long average_on = 0;

void initIO() {
  /*
    List the topics to subscribe in the array below. The UUID is automatically inserted before the topic(s) below
    Make sure to set the mqttSubTopicCount variable accordingly.
    Place your additional init code here.
  */
  espConfig.mqttSubTopic[0] = "/nothing";
  espConfig.mqttSubTopicCount = 0;
  appendSubtopic(status_topic);
  appendSubtopic(sensor_topic);

  pinMode(relay_pin, OUTPUT);
  startup = millis();
}


void IOprocessInLoop() {
  /*
     Place your code here that needs to be executed in every loop() iteration in the main
     To publish on MQTT use theis function:
     void mqttPublish(char* topic, char* text);
  */
}


bool timerCallback(void *) {
  /*
     This function is called periodically defined by TIMERVALUE, 5000ms by default
  */

  if (warmed_up == 0) {
    if (millis() > (startup + 3600000)) {
      warmed_up = 1;
    }
  }

  switch (sm_case) {
    case 1:
      average_off = 0;
      for (int i = 0; i < 100; i++) {
        average_off += analogRead(sensorPin);
      }
#ifdef DEBUG
      Serial.print("Average on OFF: ");
      Serial.println(average_off);
      Serial.println("Switching Heating ON");
#endif
      timestamp = millis();
      sm_case = 2;
      relay_state = 1;
      digitalWrite(relay_pin, relay_state);
      sprintf(sensor_string, "{\"WarmedUp\":\"%d\",\"OnValue\":\"%d\",\"OffValue\":\"%d\"}", warmed_up, average_on, average_off);
      mqttPublish(sensor_topic, sensor_string);
#ifdef DEBUG
      Serial.println(sensor_string);
#endif
    case 2:
      if (millis() > (timestamp + 60000)) {
        sm_case = 3;
      }
      break;
    case 3:
      average_on = 0;
      for (int i = 0; i < 100; i++) {
        average_on += analogRead(sensorPin);
      }
#ifdef DEBUG
      Serial.print("Average on ON: ");
      Serial.println(average_on);
      Serial.println("Switching Heating OFF");
#endif
      timestamp = millis();
      sm_case = 4;
      relay_state = 0;
      digitalWrite(relay_pin, relay_state);
    case 4:
      if (millis() > (timestamp + 90000)) {
        sm_case = 1;
      }
      break;
  }
}


void mqttReceivedCallback(char* topic, byte* payload, unsigned int length) {
  /*
     Called every time a message arrives to a subscribed MQTT topic
     If you subscribe to multiple topics please note that you need to manually select the correct topics here for your application with strcmp for example.
     char PDU[] contains the received byte array to char array so you can use strcmp and such.
  */
  char PDU[length];
  for (unsigned int i = 0; i < (length); i++) {
    PDU[i] = char(payload[i]);
    PDU[i + 1] = '\0';
  }
#ifdef DEBUG
  Serial.print("Received topic: ");
  Serial.println(topic);
  Serial.print("MQTT payload received: ");
  Serial.println(PDU);
#endif
  /*
     Place your code here that handles the received MQTT messages
  */
}


/* ======================================================================================================================================================================================================================
   Helpers, don't modify below!
*/

void mqttPublish(char* topic, char* text) {
  byte bytes[strlen(text)];
  for (unsigned int i = 0; i < strlen(text); i++) {
    bytes[i] = (byte)text[i];
  }
  client.publish(topic, bytes, sizeof(bytes));
}

void announceNodeState() {
  mqttPublish(status_topic, status_text);
#ifdef DEBUG
  Serial.println("Device status published on MQTT: ");
  Serial.println(status_text);
#endif

}

void CharToByte(char* chars, byte* bytes, unsigned int count) {
  //DO NOT DELETE this function
  for (unsigned int i = 0; i < count; i++)
    bytes[i] = (byte)chars[i];
}
