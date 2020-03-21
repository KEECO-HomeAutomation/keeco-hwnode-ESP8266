void start_mDNS() {
    if (!MDNS.begin(espConfig.hostString)) {
        espConfig.statuses.mdnsRunning = false;
#ifdef DEBUG
        Serial.println("ERROR: mDNS Responder could not start!");
#endif
    }
    else  {
      espConfig.statuses.mdnsRunning = true;
#ifdef DEBUG
        Serial.print("mDNS Running, Hoststring with ChipID: ");
        Serial.println(espConfig.hostString);
#endif
    }
}

void mdnsInLoop() {
    MDNS.update();
}
