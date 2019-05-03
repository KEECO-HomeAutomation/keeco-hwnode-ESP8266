/*Place code here that handles I/O. 
initIO() - place code here that initializes the I/Os
publishIO() - place code here to publish values over MQTT / client.publish(char* topic, byte* payload, unsigned int length);
setOutputs() - this is the callback function for the MQTT subscription. Subscriptions are defined in the MQTT file
autonomousIOHandler() / for future reference for autonomous functions
------------------------------------------------------------Do not modify comments or function names below this line!---------------------------------------------------------------------------*/

//includes


//global Variables


//generated Code
void initIO() {

}

void readIO() {

}

bool publishIO(void *) {
    digitalWrite(LED_BUILTIN, 0);
    delay(20);
    digitalWrite(LED_BUILTIN, 1);
    return true;
}

void setOutputs(char* topic, byte* payload, unsigned int length) {

}

void autonomousIOHandler () {
#ifdef AUT_IO
    //place code here that handles autonomous I/O functions
#endif
}
