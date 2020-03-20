# keeco-hwnode-ESP8266

Code that runs on ESP8266 targets converting them to hardware nodes that can connect to keeco hubs. The server-hw_node connection is MQTT, hw_node configuration is possible via a web browser.

## Getting Started

This application is a framework that can be used for various purposes. The actual code in Manage_IO.ino represents a MQTT controlled gate lock with RFID scanning. To have a different functionality you only need to change the content of Manage_IO.ino

Upload the application to your device and use {"command":"help"} in the serial interface or navigate to your device's IP address and choose from the available options.
The default password for the web interface is "12345678"

### Prerequisites

What things you need to install the software and how to install them

- Arduino IDE

- Non-shipping libraries are included with the related URL commented

- For configuring the Device to join a WiFi network you only need a web-browser 

### Installing

- Pull the project from GitHub

- Build and download you code

- Observe your device's soft AP address in the Serial Monitor

- Use {"command":"help"} via the serial port to see the list of available commands 

- Connect to your softAP (password: 12345678) and navigate to your device's IP address in your web-browser

- Or by connecting to the serial port use: {command:"wifi", "ssid":"___","password":"____"}

- Set your TLS Fingerprint via the serial port or via the webpage

- Connect your device to your infrastructure WiFi network



## Running the tests

TBD




