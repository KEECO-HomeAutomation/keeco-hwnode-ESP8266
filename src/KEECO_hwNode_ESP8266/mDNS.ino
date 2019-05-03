void start_mDNS() {
    sprintf(hostString, "KEECO_%06X", ESP.getChipId());
    if (!MDNS.begin(hostString)) {
        mdnsRunning = false;
#ifdef DEBUG
        Serial.println("ERROR: mDNS Responder could not start!");
#endif
    }
    else  {
      mdnsRunning = true;
#ifdef DEBUG
        Serial.print("mDNS Running, Hoststring with ChipID: ");
        Serial.println(hostString);
#endif
    }
}

void query_mDNS() {
    int n = MDNS.queryService("mqtt", "tcp"); // Send out query for mqtt tcp services
#ifdef DEBUG
    Serial.println("mDNS query done");
    if (n == 0) {
        Serial.println("no services found");
    } else {
        Serial.print(n);
        Serial.println(" service(s) found");
        for (int i = 0; i < n; ++i) {
            // Print details for each service found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(MDNS.hostname(i));
            Serial.print(" (");
            Serial.print(MDNS.IP(i));
            Serial.print(":");
            Serial.print(MDNS.port(i));
            Serial.println(")");
        }
    }
#endif
}

void mdnsInLoop() {
    MDNS.update();
}
