#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <timer.h>             //https://github.com/contrem/arduino-timer
#include <FS.h>
#include <ESP8266TrueRandom.h> //https://github.com/marvinroger/ESP8266TrueRandom, for the UUID generation
#include <PubSubClient.h>      //https://pubsubclient.knolleary.net/api.html 


#define DEBUG     //to enable debug purpose serial output 
#define OTA       //not yet implemented
#define AUT_IO    //to enable autonomous functions

WiFiClient wifiClient;

ESP8266WebServer webserver(80);
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


//Wifi Credentials - 32 and 64 lengths are defined by standard. Values are loaded from EEPROM
char WiFi_SSID[33] = "";
char WiFi_Password[65] = "";

//Soft AP Credentials
char AP_SSID[33] = {0};
char *AP_Password = "12345678";

//for the mDNS identification, name of the Hardware Node. Populated in the start_mDNS() function
char hostString[17] = {0};

//String that contains the data from the infoTXT file on the FS
char contentOfInfoTxt[1024];

//MQTT globals
IPAddress mqttServer(192, 168, 1, 20);
//char *mqttServer = "keeco-hub.local";

//#DeviceUUID - identifies the device at the server application
char deviceUUID[37];

//Status of WiFi connection
bool wifiIsConnected = false;

//timer for various tasks - for future scalability
auto timer = timer_create_default();

//for Browser based file manager
Dir dir;

void setup() {
  //Init Serial port
  Serial.setTimeout(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting up KEECO HW Node...");

  //Filesystem is used to send HW Node Info over HTTP
  SPIFFS.begin();
  initDirStructureOnFS();
  //Loads UUID from EEPROM
  loadDeviceUUID();

  //loading data from the infoFile on th FS
  readInfoFileFromFS();

  //loading WiFi credentials from EEPROM
  loadWifiCredentials();
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(WiFi_SSID, WiFi_Password);
  //WiFiMulti.addAP(WiFi_SSID, WiFi_Password);

  //provides interface to configure WiFi credentials at startup - "SSID,Password" format
  checkIfConfigModeReq(10000, true);

  //Webserver to enable WiFi configuration via browser
  startWebserver();

  //Setup MQTT server information
  mqttConectionSetup();
}

void loop() {
  webserverInLoop();
  mqttInLoop();
  mdnsInLoop();
  timer.tick();
  handleIO();
}
