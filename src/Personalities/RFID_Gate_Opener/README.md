# Overview of the KEECO RFID Gate Opener personality
This application can be used with the https://github.com/KEECO-HomeAutomation/KEECO-HW-Designs/tree/master/PCBs/wemos_d1_mini/KEECO_RFID_GateControl hardware design.
The code implements an MQTT over WiFi and RFID controlled gate opener device with 2 independent, 230V compatible relays

# Theory of operation
The device can
 - be switched remotely over MQTT using the relays' MQTT topics
 - read an RFID Tag, send the tag data to a home automation HUB
 - read 4 additional digital inputs (for example as a doorbell button) accessible on the I/O_Conn
 - control 2 additional outputs (as Relay3 and Relay4) accessible on the I/O_Conn
 
 Creating a simple automation in the home automation HUB the RFID Tag can be checked in a database and if found the gate opener relay can be controlled.
 
 # Usage
 Download (or clone) the basic KEECO functionality code and overwrite relevant files with the files from the RFID_Gate_Opener personality.
 
 # MQTT Topics
  - Device UUID/RFID_reader        Publishing the read RFID Tag
  - Device UUID/state              Publishing the LOCKED/UNLOCKED state of the gate
  - Device UUID/input1             Publishing the state of input 1 (0 or 1)
  - Device UUID/input2             Publishing the state of input 2 (0 or 1)
  - Device UUID/input3             Publishing the state of input 3 (0 or 1)
  - Device UUID/input4             Publishing the state of input 4 (0 or 1)

  - Device UUID/setRelay1          Controlling Relay1  Is always assigned to gate control and can be only switched on with the code (9988 by default)
  - Device UUID/setRelay2          Controlling Relay2  (0 or 1)
  - Device UUID/setRelay3          Controlling Relay3  (0 or 1)
  - Device UUID/setRelay4          Controlling Relay4  (0 or 1)
