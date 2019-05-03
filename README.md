# keeco-hwnode-ESP8266

Code that runs on ESP8266 targets converting them to hardware nodes that can connect to keeco hubs. The server-hw_node connection is MQTT, hw_node configuration is possible via a web browser.


## Getting Started

The project requires uploading the data folder to the ESP8266 device. The project includes a tools folder with a jar file. It enables an "ESP8266 Sketch Data Upload" menu item in the Arduino IDE where you can do so. Also after downloading the program itself you can download data to your device using a web browser.

Navigate to your device's IP address and choose from the available options (links).

The Manage_IO.ino is the template where you can enter your own code for interfacing with specific I/O or shields. Later on this will be automated by a build & deploy tool.
Related topics should also be updated in the MQTT.ino in the mqttSubTopic[] array. No other modification is needed to be able to use your specific hardware I/O.

### Prerequisites

What things you need to install the software and how to install them

- Arduino IDE

- Non-shipping libraries are included with the related URL commented

- On the server side you need a KEECO HUB device to enjoy full functionality.

- For configuring the Device to join a WiFi network you only need a web-browser.

- For Arduino OTA on Windows please install Bonjour service to be able to support mDNS in the background. On Mac machines this is available be default. On Linux machines you can use Avahi for example.

### Installing

- Pull the project from GitHub

- Using your Arduino IDE download the data folder by using the "ESP8266 Sketch Data Upload" menu item

- Complete the code in the Manage_IO.ino and modify the topics in the MQTT.ino

- Build and download your code

- Observe your device's soft AP address in the Serial Monitor - which is by default 192.168.4.1

- Connect to your softAP (password: 12345678) and navigate to your device's IP address in your web-browser

- Connect your device to your infrastructure WiFi network
-- It is also possible configure WiFi settings over the USB serial port during the boot phase. After the CONF_AVAIL message you have 2 seconds to send a "SSID,Password/r/n" message to your keeco hw node. The closing of the configuration period is marked with a CONF_NOT_AVAIL message. A successful configuraion is marked with a CONFIG_ACK message.

- Modify the txt file in the data folder if neccessary and reupload it - this step is needed if you are specifying a new schema

- After the first successful deployment you can use OTA update 

### Troubleshooting

- The code framework is tested with an empty Manage_IO.ino template thus should the build fail please look into the code you have entered yourself

- Arduino OTA uses mDNS to locate your device. Windows 10 might cause incorrect behavior with mDNS so try this to fix that: https://superuser.com/questions/1330027/how-to-enable-mdns-on-windows-10-build-17134





