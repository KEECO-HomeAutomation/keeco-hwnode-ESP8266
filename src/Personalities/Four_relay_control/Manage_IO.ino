/*
   The code below is divided into 4 main segments:
   initIO()                   -   This code is called during device init. Place your init code here.
   IOprocessInLoop()          -   This code is called with every iteration in the main loop/
   timerCallback()            -   This code is called every N seconds (5 by default) using the timer
   mqttReceivedCallback(char* topic, byte* payload, unsigned int length)    -    This code is called everytime a subscribed MQTT topic receives a new message
   CharToByte(char* chars, byte* bytes, unsigned int count)                 -    Helper function for MQTT publish
   announceNodeState()        -   Called during MQTT connection init. Can be called standalone at other times as well.
*/

#include <SPI.h>
#include <Wire.h>

//for the IO extender
#define SDA_PIN 4
#define SCL_PIN 5
#define INT_TIMEOUT 200

bool relay_state = false;


//mqtt content buffer
byte mqtt_send_buffer[64];
char temp_publish_topic[128];
char status_topic[16] = "/state";

//for the IO extender
const int16_t I2C_SLAVE = 0x38;
byte io_byte = 0xFF;
byte prevState = 0xFF;
unsigned long lastInputSample;



void announceNodeState() {
  mqtt_send_buffer[0] = io_byte;
  mqttPublish(status_topic, temp_publish_topic, mqtt_send_buffer, 1);
}


void initIO() {
  //DO NOT DELETE this function
  /*
    List the topics to subscribe in the array below.
    Make sure to set the mqttSubTopicCount variable accordingly.
  */
  strcpy(espConfig.mqttSubTopic[0], "/setRelay1");
  strcpy(espConfig.mqttSubTopic[1], "/setRelay2");
  strcpy(espConfig.mqttSubTopic[2], "/setRelay3");
  strcpy(espConfig.mqttSubTopic[3], "/setRelay4");

  espConfig.mqttSubTopicCount = 4;

#ifdef DEBUG
  Serial.println("Relay panel listening on /setRelay1..4");
#endif
  Wire.begin(SDA_PIN, SCL_PIN); // join i2c bus (address optional for master)
  announceNodeState();
}

void IOprocessInLoop() {
  //DO NOT DELETE this function
}


bool timerCallback(void *) {
  //DO NOT DELETE this function
}


void mqttReceivedCallback(char* topic, byte* payload, unsigned int length) {
  //DO NOT DELETE this function
  unsigned int iter = 0;
  unsigned int topic_length = strlen(topic);
  int subtopic_ptr = -1;
  char sub_topic[32];
  bool relaycmd = false;

  while (iter < topic_length) {
    if (topic[(topic_length - iter)] == '/') {
      subtopic_ptr = topic_length - iter;
      iter = topic_length + 1;
    }
    else {
      iter++;
    }
  }
  for (int i = 0; i < (topic_length - subtopic_ptr); i++) {
    sub_topic[i] = topic[subtopic_ptr + i];
    sub_topic[i + 1] = '\0';
  }
#ifdef DEBUG
  Serial.print("Received topic:");
  Serial.println(topic);
  Serial.println("Subtopic:");
  Serial.println(sub_topic);
#endif

  char PDU[length];
  for (unsigned int i = 0; i < (length); i++) {
    PDU[i] = char(payload[i]);
    PDU[i + 1] = '\0';
  }
#ifdef DEBUG
  Serial.println("MQTT payload received:");
  Serial.println(PDU);
#endif
  if (strcmp(sub_topic, "/setRelay1\0") == 0) {
    relaycmd = true;
    if (PDU[0] == '1') {
      io_byte = io_byte & B11111110;
    }
    if (PDU[0] == '0') {
      io_byte = io_byte | B00000001;
    }
    mqtt_send_buffer[0] = getInputState((char)io_byte, 0);
    mqttPublish("/setRelay1/status", temp_publish_topic, mqtt_send_buffer, 1);
  }
  if (strcmp(sub_topic, "/setRelay2\0") == 0) {
    relaycmd = true;
    if (PDU[0] == '1') {
      io_byte = io_byte & B11111101;
    }
    if (PDU[0] == '0') {
      io_byte = io_byte | B00000010;
    }
    mqtt_send_buffer[0] = getInputState((char)io_byte, 1);
    mqttPublish("/setRelay2/status", temp_publish_topic, mqtt_send_buffer, 1);
  }
  if (strcmp(sub_topic, "/setRelay3\0") == 0) {
    relaycmd = true;
    if (PDU[0] == '1') {
      io_byte = io_byte & B11111011;
    }
    if (PDU[0] == '0') {
      io_byte = io_byte | B00000100;
    }
    mqtt_send_buffer[0] = getInputState((char)io_byte, 2);
    mqttPublish("/setRelay3/status", temp_publish_topic, mqtt_send_buffer, 1);
  }
  if (strcmp(sub_topic, "/setRelay4\0") == 0) {
    relaycmd = true;
    if (PDU[0] == '1') {
      io_byte = io_byte & B11110111;
    }
    if (PDU[0] == '0') {
      io_byte = io_byte | B00001000;
    }
    mqtt_send_buffer[0] = getInputState((char)io_byte, 3);
    mqttPublish("/setRelay4/status", temp_publish_topic, mqtt_send_buffer, 1);
  }
  if (relaycmd) {
    Wire.beginTransmission(I2C_SLAVE); // transmit to device #8
    Wire.write(io_byte);              // sends one byte
    Wire.endTransmission();
#ifdef DEBUG
    Serial.println("Relay switched due to MQTT command");
    Serial.println(io_byte, BIN);
#endif
  }
}

byte getInputState(char value, int pin) {
  char temp = (value >> pin) & 1;
  if (temp == 0) {
    return (byte)'1';
  }
  else {
    return (byte)'0';
  }
}

void CharToByte(char* chars, byte* bytes, unsigned int count) {
  //DO NOT DELETE this function
  for (unsigned int i = 0; i < count; i++)
    bytes[i] = (byte)chars[i];
}
