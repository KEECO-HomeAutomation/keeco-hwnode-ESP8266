# keeco-hwnode-ESP8266
Code that runs on ESP8266 targets converting them to hardware nodes that can connect to keeco hubs.

On first startup the device creates and stores a UUID in the EEPROM. Later on the same UUID is used.

During the startup, after the CONFIG_AVAIL message is transmitted over the serial port it is possible to configure the WiFi credentials using "SSID,Password" format.
The configuration is possible until the CONFIG_NOT_AVAIL message.

The hardware end node appears as a SoftAP where WiFi credentials can be set. SSID format: KEECO_AP_*ChipID*

Hardware Node information can be requested via HTTP:

/info - schema file descriptor for hardware end node characteristics

/uuid - UUID that identifies the hardware end node

/edit - Browser based file manager

All of the above + WiFi settings can be reached from the root page.


Requested includes are identified in the code with URLs
