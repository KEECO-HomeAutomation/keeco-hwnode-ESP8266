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
#include <MFRC522.h>
#include <Wire.h>

//for the MFC522 RFID reader
#define SS_PIN 15
#define RST_PIN 16

//for the IO extender
#define SDA_PIN 4
#define SCL_PIN 5
#define INT_TIMEOUT 200

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

unsigned long RFID_timing;
bool relay_state = false;

// Init array that will store new NUID
String nuidPICC[4];

//mqtt content buffer
byte mqtt_send_buffer[64];
char temp_publish_topic[128];
char rfid_topic[16] = "/RFID_reader";
char status_topic[16] = "/state";
char input1_topic[16] = "/input1";
char input2_topic[16] = "/input2";
char input3_topic[16] = "/input3";
char input4_topic[16] = "/input4";
char locked_text[] = "LOCKED";
char unlocked_text[] = "UNLOCKED";

char *relay_code = "9988";

//for the IO extender
const int16_t I2C_SLAVE = 0x38;
byte io_byte = 0xFF;

//for Digital Inputs
const byte interruptPin = 3;
volatile bool ext_interrupt = false;
unsigned long int_timestamp;

void announceNodeState() {
  CharToByte(locked_text, mqtt_send_buffer, 6);
  mqttPublish(status_topic, temp_publish_topic, mqtt_send_buffer, 6);
}

ICACHE_RAM_ATTR void setintflag() {
  ext_interrupt = true;
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
  Serial.println("RFID Topic publishing:");
  appendSubtopicToNew(rfid_topic, temp_publish_topic);
  Serial.println(temp_publish_topic);
#endif
  announceNodeState();

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Wire.begin(SDA_PIN, SCL_PIN); // join i2c bus (address optional for master)
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), setintflag, FALLING);
  int_timestamp = millis();
}

void IOprocessInLoop() {
  //DO NOT DELETE this function
  readRFID();
  if (relay_state) {
    if (RFID_timing < millis()) {
      relay_state = false;
      io_byte = io_byte | 1;
      Wire.beginTransmission(I2C_SLAVE); // transmit to device #8
      Wire.write(io_byte);              // sends one byte
      Wire.endTransmission();
      //digitalWrite(relay_pin, LOW);
      Serial.println("Relay switched off after timeout");
      CharToByte(locked_text, mqtt_send_buffer, 6);
      mqttPublish(status_topic, temp_publish_topic, mqtt_send_buffer, 6);
    }
  }
  interruptInLoop();
}

bool readRFID() {
  String result = "";

  if ( ! rfid.PICC_IsNewCardPresent())
    return false;
  if ( ! rfid.PICC_ReadCardSerial())
    return false;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    result =  "{\"status\":\"error\",\"result\":\"invalidcard\"}";
  }
  else {
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = String(rfid.uid.uidByte[i], HEX);
    }
    result = "{\"status\":\"read\",\"result\":\"" + nuidPICC[0] + nuidPICC[1] + nuidPICC[2] + nuidPICC[3] + "\"}";
  }
  Serial.println("The Result is: ");
  Serial.println(result);
  result.getBytes(mqtt_send_buffer, 64);
  mqttPublish(rfid_topic, temp_publish_topic, mqtt_send_buffer, result.length());
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}


bool timerCallback(void *) {
  //DO NOT DELETE this function
  if (ext_interrupt) {
    Serial.println("interrupt happened");
  }
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
    if (strcmp(PDU, relay_code) == 0) {
      io_byte = io_byte & B11111110;
      relaycmd = true;
      CharToByte(unlocked_text, mqtt_send_buffer, 8);
      mqttPublish(status_topic, temp_publish_topic, mqtt_send_buffer, 8);
      RFID_timing = millis() + 3000;
      relay_state = true;
      Serial.println("GateRelay switched on due to MQTT command");
    }
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

void CharToByte(char* chars, byte* bytes, unsigned int count) {
  //DO NOT DELETE this function
  for (unsigned int i = 0; i < count; i++)
    bytes[i] = (byte)chars[i];
}

void interruptInLoop() {
  if ((millis() - int_timestamp) > INT_TIMEOUT) {
    if (ext_interrupt) {
      Serial.println("interrupt happened");
      int_timestamp = millis();
      Wire.requestFrom(I2C_SLAVE, 1);
      char c = Wire.read();
      Serial.println(c, BIN);
      mqtt_send_buffer[0] = getInputState(c, 4);
      mqttPublish(input1_topic, temp_publish_topic, mqtt_send_buffer, 1);
      mqtt_send_buffer[0] = getInputState(c, 5);
      mqttPublish(input2_topic, temp_publish_topic, mqtt_send_buffer, 1);
      mqtt_send_buffer[0] = getInputState(c, 6);
      mqttPublish(input3_topic, temp_publish_topic, mqtt_send_buffer, 1);
      mqtt_send_buffer[0] = getInputState(c, 7);
      mqttPublish(input4_topic, temp_publish_topic, mqtt_send_buffer, 1);
      ext_interrupt = false;
    }
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
