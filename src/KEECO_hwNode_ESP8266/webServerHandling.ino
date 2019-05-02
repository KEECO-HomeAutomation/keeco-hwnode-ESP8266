void startWebserver() {

    //WiFi and HW NodeInfo related pages
    webserver.on("/", handleRoot);
    webserver.on("/wifisave", handleWifiSave);
    webserver.on("/info", handleNodeInfo);
    webserver.on("/uuid", handleUuidRequest);

    //FS Browser related pages
    webserver.on("/list", HTTP_GET, handleFileList);                                         //list directory
    webserver.on("/edit", HTTP_GET, []() {                                                   //load editor
        if (!handleFileRead("/edit.htm")) webserver.send(404, "text/plain", "FileNotFound");
    });
    webserver.on("/edit", HTTP_PUT, handleFileCreate);                                       //create file
    webserver.on("/edit", HTTP_DELETE, handleFileDelete);                                    //delete file
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    webserver.on("/edit", HTTP_POST, []() {
        webserver.send(200, "text/plain", "");
    }, handleFileUpload);

    //called when the url is not defined here
    //use it to load content from SPIFFS
    webserver.onNotFound([]() {
        if (!handleFileRead(webserver.uri()))
            webserver.send(404, "text/plain", "FileNotFound");
    });
    //Start the WebServer
    webserver.begin();
#ifdef DEBUG
    Serial.println("Webserver running...");
#endif
}

void webserverInLoop() {
    webserver.handleClient();
}
