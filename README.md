# keeco-hwnode-ESP8266

Code that runs on ESP8266 targets converting them to hardware nodes that can connect to keeco hubs. Practically it is a universal web configurable MQTT device. 

## Getting Started

This application is a framework that can be used for various purposes. The actual code in Manage_IO.ino represents a MQTT controlled gate lock with RFID scanning. To have a different functionality you only need to change the content of Manage_IO.ino


Main features:
- OTA Enabled
- TLS fingerprint validation for MQTT communication
- mDNS name resolving for the MQTT server
- Connect to infrastructure wifi. Based on the availability of infrastructure wifi the SoftAP is auto enabled/disabled
- Serial port (over USB) interface to change settings (use {"command":"help"} to see available commands)
- Web interface for configuration (default password "12345678"). Automatically logged out in 5 minutes after login.
- SPIFFS to store configuration in JSON file

Upload the application to your device and use {"command":"help"} in the serial interface or navigate to your device's IP address and choose from the available options.
The default password for the web interface is "12345678"


### Prerequisites

What things you need to install the software and how to install them

- Arduino IDE
- Non-shipping libraries are included with the related URL commented
- SPIFFS is however used you do not need to upload any file
- For configuring the Device to join a WiFi network you only need a web-browser 

### Installing

- Pull the project from GitHub
- Build and download you code
- Observe your device's soft AP address in the Serial Monitor
- Use {"command":"help"} via the serial port to see the list of available commands 
- Connect to your softAP (password: 12345678) and navigate to your device's IP address (default: 192.168.4.1) in your web-browser
- Or by connecting to the serial port use: {command:"wifi", "ssid":"....", "password":"...."}
- Set your TLS Fingerprint via the serial port or via the webpage
- Connect your device to your infrastructure WiFi network

### Screenshots

Login Page

![Login page](https://i.imgur.com/sRYtBYO.jpg)

General Settings

![General Settings page](https://imgur.com/zpNqMee.jpg)

WiFi Settings

![WiFi Settings page](https://imgur.com/suEIvZn.jpg)

Serial Interface

![Serial Interface](https://imgur.com/PMPGhJw.jpg)



