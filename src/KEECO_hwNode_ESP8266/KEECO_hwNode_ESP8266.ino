/*
 * KEECO HW Node application software
 * Version 3.2
 * Developed by https://github.com/litechniks
 * 
 * Only Manage_IO needs to be modified to implement your application
 * The usecase in this code is a gate lock functionality with and RFID reader that is operated over MQTT
 * 
 * 
 */

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <timer.h>             //https://github.com/contrem/arduino-timer
#include <FS.h>
#include <ESP8266TrueRandom.h> //https://github.com/marvinroger/ESP8266TrueRandom, for the UUID generation
#include <PubSubClient.h>      //https://pubsubclient.knolleary.net/api.html
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "configFileHandler.h"


#define DEBUG     //to enable debug purpose serial output 
#define OTA       //not yet implemented

ESP8266WebServer webserver(80);

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

ConfigurationHandler espConfig;

//timer for various tasks - for future scalability
auto timer = timer_create_default();


/*
  Initializing the KEECO HW Node
  Serial Timeout is needed for the serial command terminal
  SPIFFS - is used to store the configuration data - SSID, Password, mqtt_server, UUID
  espConfig - global object storing the configuration variables
  initWifiOnBoot - try to connect to Infrastructure WiFi (60 sec timeout). If not successful start AP mode. Later if disconnected from STA then AP reactivates. Also starts mDNS.
  initWebserver - webserver to set configuration parameters
  initMqtt - connect to the set mqtt server. Name is resolved with mDNS. Set subscriptions.
  initIO - place your custom init code in this function
  InitOTA - initializing OTA
  timer - setup a timer that calls publishIO() - place your periodically called code there

*/
void setup() {
  Serial.setTimeout(10);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting up KEECO HW Node...");
  SPIFFS.begin();
  Serial.println("[=_______]");
  espConfig.initConfiguration();
  Serial.println("[==______]");
  initWifiOnBoot();
  Serial.println("[===_____]");
  initWebserver();
  Serial.println("[====____]");
  initMqtt();
  Serial.println("[=====___]");
  initIO();
  Serial.println("[======__]");
  InitOTA();
  Serial.println("[=======_]");
  timer.every(5000, timerCallback);
  Serial.println("[========]");
  Serial.println("Welcome to KEECO HW Node version 3.2!");
  Serial.println("Send {\"command\":\"help\"} to see a list of commands");
}

void loop() {
  timer.tick();
  webserverInLoop();
  mqttInLoop();
  mdnsInLoop();
  IOprocessInLoop();
  OTAInLoop();
  espConfig.serialCmdCheckInLoop();
}
