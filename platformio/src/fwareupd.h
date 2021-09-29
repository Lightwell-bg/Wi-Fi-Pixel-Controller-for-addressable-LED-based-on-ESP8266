static const char successResponse[] PROGMEM = 
  "<META http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...";

String updateError = "";
String getUpdaterError();

void init_firmWareUpdate(void) {
    //String updateError = "";
    HTTP.on("/update", HTTP_POST, [](){ //For Update firmware and data.bin
    /*HTTP.sendHeader("Connection", "close");
      HTTP.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();*/
      if (Update.hasError()) {
        HTTP.send(200, F("text/html"), String(F("Update error: ")) + updateError);
      } else {
        HTTP.client().setNoDelay(true);
        HTTP.send_P(200, PSTR("text/html"), successResponse);
        delay(100);
        HTTP.client().stop();
        ESP.restart();
      }
  },[]() {
    HTTPUpload& upload = HTTP.upload();
    if(upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      //Serial.print("Size: "); Serial.println(upload.totalSize);
      //size_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      size_t maxSketchSpace = ((size_t) &_FS_end - (size_t) &_FS_start);
      //size_t content_len = upload.totalSize;
      //int cmd = (upload.filename.indexOf("spiffs") > -1) ? 100 : 0; //U_FS : U_FLASH;
      int cmd = (upload.filename.indexOf("littlefs") > -1 || upload.filename.indexOf("spiffs") > -1) ? U_FS : U_FLASH; //U_FS : U_FLASH;
      //if(!Update.begin(maxSketchSpace,cmd)) {//start with max available size
      if(!Update.begin(maxSketchSpace, cmd)) {//start with max available size
              //Update.printError(Serial);
              updateError = getUpdaterError();
      }
    }
    else if(upload.status == UPLOAD_FILE_WRITE) {
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        //Update.printError(Serial);
        updateError = getUpdaterError();
      }
    }
    else if(upload.status == UPLOAD_FILE_END) {
      if(Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        //Update.printError(Serial);
        updateError = getUpdaterError();
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });  
}

String getUpdaterError() {
  Update.printError(Serial);
  StreamString str;
  Update.printError(str);
  return String(str.c_str());
}