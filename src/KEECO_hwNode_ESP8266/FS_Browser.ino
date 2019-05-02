/*
    FSWebServer - Example WebServer with SPIFFS backend for esp8266
    Copyright (c) 2015 Hristo Gochkov. All rights reserved.
    This file is part of the ESP8266WebServer library for Arduino environment.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    upload the contents of the data folder with MkSPIFFS Tool ("ESP8266 Sketch Data Upload" in Tools menu in Arduino IDE)
    or you can upload the contents of a folder if you CD in that folder and run the following command:
    for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp8266fs.local/edit; done

    access the sample web page at http://esp8266fs.local
    edit the page by going to http://esp8266fs.local/edit
*/

File fsUploadFile;

//format bytes
String formatBytes(size_t bytes) {
    if (bytes < 1024) {
        return String(bytes) + "B";
    } else if (bytes < (1024 * 1024)) {
        return String(bytes / 1024.0) + "KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    } else {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

String getContentType(String filename) {
    if (webserver.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {
#ifdef DEBUG
    Serial.println("handleFileRead: " + path);
#endif
    if (path.endsWith("/")) path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        if (SPIFFS.exists(pathWithGz))
            path += ".gz";
        File file = SPIFFS.open(path, "r");
        size_t sent = webserver.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

void handleFileUpload() {
    if (webserver.uri() != "/edit") return;
    HTTPUpload& upload = webserver.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
#ifdef DEBUG
        Serial.print("handleFileUpload Name: "); Serial.println(filename);
#endif
        fsUploadFile = SPIFFS.open(filename, "w");
        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile)
            fsUploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile)
            fsUploadFile.close();
#ifdef DEBUG
        Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
#endif
    }
}

void handleFileDelete() {
    if (webserver.args() == 0) return webserver.send(500, "text/plain", "BAD ARGS");
    String path = webserver.arg(0);
#ifdef DEBUG
    Serial.println("handleFileDelete: " + path);
#endif
    if (path == "/")
        return webserver.send(500, "text/plain", "BAD PATH");
    if (!SPIFFS.exists(path))
        return webserver.send(404, "text/plain", "FileNotFound");
    SPIFFS.remove(path);
    webserver.send(200, "text/plain", "");
    path = String();
}

void handleFileCreate() {
    if (webserver.args() == 0)
        return webserver.send(500, "text/plain", "BAD ARGS");
    String path = webserver.arg(0);
#ifdef DEBUG
    Serial.println("handleFileCreate: " + path);
#endif
    if (path == "/")
        return webserver.send(500, "text/plain", "BAD PATH");
    if (SPIFFS.exists(path))
        return webserver.send(500, "text/plain", "FILE EXISTS");
    File file = SPIFFS.open(path, "w");
    if (file)
        file.close();
    else
        return webserver.send(500, "text/plain", "CREATE FAILED");
    webserver.send(200, "text/plain", "");
    path = String();
}

void handleFileList() {
    if (!webserver.hasArg("dir")) {
        webserver.send(500, "text/plain", "BAD ARGS");
        return;
    }

    String path = webserver.arg("dir");
#ifdef DEBUG
    Serial.println("handleFileList: " + path);
#endif
    Dir dir = SPIFFS.openDir(path);
    path = String();

    String output = "[";
    while (dir.next()) {
        File entry = dir.openFile("r");
        if (output != "[") output += ',';
        bool isDir = false;
        output += "{\"type\":\"";
        output += (isDir) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += String(entry.name()).substring(1);
        output += "\"}";
        entry.close();
    }

    output += "]";
    webserver.send(200, "text/json", output);
}

void initDirStructureOnFS()  {
    dir = SPIFFS.openDir("/");
    while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
#ifdef DEBUG
        Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
#endif
    }
#ifdef DEBUG
    Serial.printf("\n");
#endif
}
