# keeco-hwnode-ESP8266

Code that runs on ESP8266 targets converting them to hardware nodes that can connect to keeco hubs. The server-hw_node connection is MQTT, hw_node configuration is possible via a web browser.

## Getting Started

The project requires uploading the data folder to the ESP8266 device. The project includes a tools folder with a jar file. It enables an "ESP8266 Sketch Data Upload" menu item in the Arduino IDE where you can do so. Also after downloading the program itself you can download data to your device using a web browser.

Navigate to your device's IP address and choose from the available options (links).

### Prerequisites

What things you need to install the software and how to install them

- Arduino IDE

- Non-shipping libraries are included with the related URL commented

- On the server side you need a KEECO HUB device to enjoy full functionality.

- For configuring the Device to join a WiFi network you only need a web-browser 

### Installing

- Pull the project from GitHub

- Using your Arduino IDE download the data folder by using the "ESP8266 Sketch Data Upload" menu item

- Build and download you code

- Observe your device's soft AP address in the Serial Monitor

- Connect to your softAP (password: 12345678) and navigate to your device's IP address in your web-browser

- Connect your device to your infrastructure WiFi network

- Modify the txt file in the data folder if neccessary and reupload it - this step is needed if you are specifying a new schema


## Running the tests

TBD




