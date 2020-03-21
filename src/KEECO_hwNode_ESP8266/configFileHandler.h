#ifndef configFileHandler_H
#define configFileHandler_H

class ConfigurationHandler {
  public:
    struct STA
    {
      char ssid[33];                      //WiFi STA mode SSID - 32 chars max are defined by standard
      char password[65];                  //WiFi STA mode Passsword - 64 chars are defined by standard
    };
    STA wifiSTA;
    struct AP
    {
      char ssid[33];                      //WiFi SoftAP mode SSID - 32 chars max are defined by standard
      char password[65];                  //WiFi SoftAP mode Passsword - 64 chars are defined by standard
    };
    AP wifiAP;
    struct stats
    {
      bool mdnsRunning;                   //mDNS successfully started
      bool wifiIsConnected;               //WiFi is connected to Infrastructure Network
      bool softApRunning;                 //Device functions as an AP
      bool authenticated;                 //webserver authentication
      long auth_timestamp;                //Timestamp when the authentication happened. Being used to logout automatically after 5 minutes.
    };
    stats statuses;

    char hostString[17];                //for the mDNS identification, name of the Hardware Node. Populated in the start_mDNS() function
    char *mqttServer;                   //MQTT server address
    char *mqttSubTopic[16];             //MQTT topics to subscribe
    int mqttSubTopicCount;              //number of MQTT subscribe topics
    char deviceUUID[37];                //#DeviceUUID - identifies the device at the server application
    char fingerprint[64];
    ConfigurationHandler(void);         //Constructor - Assigning values to variables
    char* getContentSerd();             //Get configurations JSON string
    void initConfiguration(void);       //Load configuration from file and init file if originally not found
    void storeWifiCredentials(void);    //Save WiFi credentials to file
    void storeFingerprint(void);        //Store new TLS fingerprint
    void storeMqttSrv(void);
    void storeUUID(void);
    void saveConfig(void);              //Save config to file
    void serialCmdCheckInLoop(void);    //Check if serial port has received something and process

  private:
    int fileSize;                       //Size of config file
    char* defaultJson;                  //Default JSON string if file was not found
    byte uuidNumber[16];                //UUID byte array if file was not found
    String uuidStr;                     //UUID string for the device
    char tempContentSerd[256];          //JSON parsing uses the original string as buffer thus copies are created manually. This variable stores the current readable string
    char tempContentDeserd[256];
    char serialData[100];               //Raw data received from the serial port
    StaticJsonDocument<200> configDoc;  //Parser object to work with the configuration data
    DeserializationError configError;   //Error for configDoc parsing
    StaticJsonDocument<200> configUartDoc;    //Parser object to work with the commands from the serial port
    DeserializationError error;         //Error for the doc parsing

    bool parseJsonPacket(void);         //Function that parses the received JSON data
};

#endif
