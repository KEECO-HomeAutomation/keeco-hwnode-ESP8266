//Place code here that handles I/O
//ioValues[48]; Assign 3 bytes per I/O item, max 16 I/O items per node. 1st byte = local ID, 2nd-3rd byte = 16 bit I/O value

void handleIO() {
  //client.publish(char* topic, byte* payload, unsigned int length);
}

void setOutputs(char* topic, byte* payload, unsigned int length) {
  //place code here that updates outputs
}

void autonomousIOHandler () {
#ifdef AUT_IO
  //place code here that handles autonomous I/O functions
#endif
}
