#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <EEPROM.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <timer.h>             //https://github.com/contrem/arduino-timer
#include <FS.h>
#include <ESP8266TrueRandom.h> //https://github.com/marvinroger/ESP8266TrueRandom, for the UUID generation
#include <ArduinoMqttClient.h> //https://github.com/arduino-libraries/ArduinoMqttClient
 

#define DEBUG     //to enable debug purpose serial output 
#define OTA       //not yet implemented
#define AP        //to enable Soft Access Point
#define AUT_IO    //to enable autonomous functions

 ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer webserver(80);
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


//Wifi Credentials - 32 and 64 lengths are defined by standard. Values are loaded from EEPROM
char WiFi_SSID[33] = "";
char WiFi_Password[65] = "";

//Soft AP Credentials
char AP_SSID[33] = {0};
char *AP_Password = "12345678";

//for the mDNS identification, name of the Hardware Node
char hostString[17] = {0};  

//MQTT globals
const char broker[] = "test.mosquitto.org";
const char topic[]  = "arduino/simple";

//#DeviceUUID - identifies the device at the server application
char deviceUUID[37];        

//Status of WiFi connection
bool wifiIsConnected = false;

//timer for various tasks - for future scalability
auto timer = timer_create_default();


void setup() {  
  Serial.setTimeout(1000);
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Starting up KEECO HW Node...");

  //Filesystem is used to send HW Node Info over HTTP
  SPIFFS.begin();
  
  loadWifiCredentials();
  loadDeviceUUID();
  WiFiMulti.addAP(WiFi_SSID, WiFi_Password);
  WiFi.onEvent(WiFiEvent);
  
  //provides interface to configure WiFi credentials at startup - "SSID,Password" format
  checkIfConfigModeReq(10000, true);
    
  configureAPSettings();
  startWebserver();
  connectToMqttBroker();
  initializeMqttSession();
}

void loop() {
  delay(1000);
  webserverInLoop();
  handleIO();
  mqttInLoop(); 
  timer.tick();
  Serial.println("Running...");
}
