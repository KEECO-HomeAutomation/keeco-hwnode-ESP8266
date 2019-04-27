void readInfoFileFromFS() {
  String tempContent;
  int tempContentLength;
  File infoFile = SPIFFS.open("/hwnode_info.txt", "r");
  if (!infoFile) {
#ifdef DEBUG
    Serial.println("Config File not found");
#endif
  }
  else {
#ifdef DEBUG
    Serial.println("Info TXT has been successfully opened");
#endif
    tempContent = infoFile.readString();
    tempContent.replace("UUID_PLACEHOLDER", deviceUUID);        //to replace placeholder with actual UUID
    tempContentLength = tempContent.length();
    tempContent.toCharArray(contentOfInfoTxt, 1024);
    Serial.println("InfoTXT length");
    Serial.println(tempContentLength);
    Serial.println("InfoTXT readout");
    Serial.println(contentOfInfoTxt);
  }
  infoFile.close();
}
