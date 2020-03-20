/*
   The code below is divided into 4 main segments:
   initIO()                   -   This code is called during device init. Place your init code here.
   IOprocessInLoop()          -   This code is called with every iteration in the main loop/
   timerCallback()            -   This code is called every N seconds (5 by default) using the timer
   mqttReceivedCallback(char* topic, byte* payload, unsigned int length)    -    This code is called everytime a subscribed MQTT topic receives a new message
   CharToByte(char* chars, byte* bytes, unsigned int count)                 -    Helper function for MQTT publish
*/

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 4
#define RST_PIN 5


MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

unsigned long RFID_timing;
bool relay_state = false;

// Init array that will store new NUID
String nuidPICC[4];

//mqtt content buffer
byte mqtt_send_buffer[64];
char rfid_topic[128] = "/RFID_reader";
char status_topic[128] = "/state";
char locked_text[] = "LOCKED";
char unlocked_text[] = "UNLOCKED";

int relay_pin = 15;
char *relay_code = "9988";

void announceLockState() {
  CharToByte(locked_text, mqtt_send_buffer, 6);
  client.publish(status_topic, mqtt_send_buffer, 6);
}

void initIO() {
  //DO NOT DELETE this function
  /*
    List the topics to subscribe in the array below.
    Make sure to set the mqttSubTopicCount variable accordingly.
  */
  espConfig.mqttSubTopic[0] = "/setRelay1";
  espConfig.mqttSubTopicCount = 1;

  pinMode(relay_pin, OUTPUT); //relay output
  appendSubtopic(rfid_topic);
  appendSubtopic(status_topic);
  Serial.println("RFID Topic publishing:");
  Serial.println(rfid_topic);
  announceLockState();

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}


void IOprocessInLoop() {
  //DO NOT DELETE this function
  readRFID();
  if (relay_state) {
    if (RFID_timing < millis()) {
      relay_state = false;
      digitalWrite(relay_pin, LOW);
      Serial.println("Relay switched off after timeout");
      CharToByte(locked_text, mqtt_send_buffer, 6);
      client.publish(status_topic, mqtt_send_buffer, 6);
    }
  }
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
  Serial.println(F("The Result is: "));
  Serial.println(result);
  result.getBytes(mqtt_send_buffer, 64);
  client.publish(rfid_topic, mqtt_send_buffer, result.length());

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}


bool timerCallback(void *) {
  //DO NOT DELETE this function
}


void mqttReceivedCallback(char* topic, byte* payload, unsigned int length) {
  //DO NOT DELETE this function
  char PDU[length];
  for (unsigned int i = 0; i < (length); i++) {
    PDU[i] = char(payload[i]);
    PDU[i + 1] = '\0';
  }
  Serial.println("MQTT payload received:");
  Serial.println(PDU);
  if (strcmp(PDU, relay_code) == 0) {
    digitalWrite(relay_pin, HIGH);
    CharToByte(unlocked_text, mqtt_send_buffer, 8);
    client.publish(status_topic, mqtt_send_buffer, 8);
    RFID_timing = millis() + 3000;
    relay_state = true;
    Serial.println("Relay switched on due to MQTT command");
  }
}

void CharToByte(char* chars, byte* bytes, unsigned int count) {
  //DO NOT DELETE this function
  for (unsigned int i = 0; i < count; i++)
    bytes[i] = (byte)chars[i];
}
