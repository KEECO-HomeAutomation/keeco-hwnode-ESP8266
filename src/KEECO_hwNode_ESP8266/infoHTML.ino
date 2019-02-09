String readInfoFileFromFS(){
    File infoFile = SPIFFS.open("/hwnode_info.txt", "r");
    if (!infoFile){
          Serial.println("Config File not found");
          return "No infoFile available";
    }
    else {
      String content = infoFile.readString();
      return content;
    }
    infoFile.close();
}
