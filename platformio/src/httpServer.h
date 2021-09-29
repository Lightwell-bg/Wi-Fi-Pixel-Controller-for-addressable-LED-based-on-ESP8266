void init_SSDP(void);
void handleNotFound();
String getContentType(String filename);
bool handleFileRead(String path);
void handle_ConfigJSON();
void handle_Language();
void handle_Set_Ssid();
void handle_time_zone();
void handle_ntp_server();
void handle_Time();
void handle_TimeNew();
void handle_Restart();
void handle_resetConfig();
void handle_set_ledStrip();

void init_HTTPServer(void) {
    HTTP.serveStatic("/", LittleFS, "/index.html");
    HTTP.on("/configs.json", handle_ConfigJSON); // create configs.json for sending to WEB interface
    HTTP.on("/lang", handle_Language);
    HTTP.on("/ssid", handle_Set_Ssid);     // Set WiFi config and SSDP
    HTTP.on("/Time", handle_Time);     // Sync Time
    HTTP.on("/TimeZone", handle_time_zone);    // Set time zone
    HTTP.on("/setntpserver", handle_ntp_server); // Set sync NTP
    HTTP.on("/TimeNew", handle_TimeNew);     // Set new time 
    HTTP.on("/restart", handle_Restart);   // reset ESP
    HTTP.on("/resetConfig", handle_resetConfig);
    HTTP.on("/set_ledStrip", handle_set_ledStrip); 

    HTTP.onNotFound([]() {handleNotFound();});  // Captive_Portal ********************************       
    HTTP.begin();  
}

// Captive_Portal ********************************
bool captivePortal() {
  if (! HTTP.hostHeader().equals(WiFi.softAPIP().toString())) {
    HTTP.sendHeader(F("Location"), String(F("http://")) + WiFi.softAPIP().toString() + String(F("/wifi.html")), true);
    HTTP.send_P(302, "text/plain", NULL, 0);
    return true;
  }
  return false;
}

void handleNotFound() {
    if (WiFi.status() != WL_CONNECTED) {
        if (captivePortal()) return;
    }
    if (! handleFileRead(HTTP.uri()))
        HTTP.send_P(404, "text/plain", "FileNotFound");
}

void handle_ConfigJSON() { 
    struct tm tnow = myESPTime.getTimeStruct();
    String root = "{}";  
    DynamicJsonDocument jsonDoc(5096);//4096
    DeserializationError error =  deserializeJson(jsonDoc, root);
    if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return;
    }     
    JsonObject json = jsonDoc.as<JsonObject>(); 
    json["cVersion"] = cVersion;   
    json["SSDP"] = myWIFI.getNameSSDP();
    json["ssidAP"] = myWIFI.getNameAPSSID();
    json["passwordAP"] = myWIFI.getPassAPSSID();
    json["ssid"] = myWIFI.getNameSSID();
    json["password"] = myWIFI.getPassSSID();
    json["ip"] = myWIFI.getDevStatusIP();
    json["time"] = myESPTime.getTimeStr(true);
    json["date"] = myESPTime.getDateStr();
    json["time_h"] = String(tnow.tm_hour);
    json["time_m"] = String(tnow.tm_min);
    json["time_s"] = String(tnow.tm_sec); 
    json["date_day"] = tnow.tm_mday;
    json["date_month"] = tnow.tm_mon + 1;
    json["date_year"] = tnow.tm_year + 1900;
    json["timezone"] = timezone;
    json["isDayLightSaving"] = (isDayLightSaving?"checked":"");
    json["ntpserver"] = sNtpServerName; 
    json["use_sync"] = typeSync;
    json["lang"] = (lang==0?"RU":lang==1?"BG":lang==2?"EN":"EN");
    json["global_start"] = global_start; json["global_stop"] = global_stop; 
    json["brightd"] = brightd; json["brightn"] = brightn;
    json["brauto"] = (brauto?"checked":"");
    json["dmodefrom"] = dmodefrom; json["dmodeto"] = dmodeto;
    json["ledStripMode"] = ledStripMode;
    json["rndledStripMode"] = (rndledStripMode?"checked":"");
    

    root = "";
    serializeJson(json, root);
    HTTP.send(200, "text/json", root);
}

void handle_Language() {               
    if(HTTP.arg("lang") == "RU" && lang !=0) {
        lang = 0;
    }
    else if(HTTP.arg("lang") == "BG" && lang !=1) {
        lang = 1;
    }
    else if(HTTP.arg("lang") == "EN" && lang !=2) {
        lang = 2;
    }
    else {
        lang = 2;
    }
    Serial.print("Set lang: ");    Serial.println(HTTP.arg("lang").c_str());
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
}

void handle_Set_Ssid() {
    ssid = HTTP.arg("ssid").c_str();            
    password = HTTP.arg("password").c_str();   
    ssidAP = HTTP.arg("ssidAP").c_str();        
    passwordAP = HTTP.arg("passwordAP").c_str(); 
    SSDP_Name = HTTP.arg("ssdp").c_str();
    if (passwordAP.length() != 0) {
        if (passwordAP.length() < 8 || passwordAP.length() > 63)  passwordAP = "";  
    }
    //if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63) {
    myWIFI.setConfigWIFI(ssid.c_str(), password.c_str(), SSDP_Name.c_str(), ssidAP.c_str(), passwordAP.c_str()); 
    Serial.println("Set SSID: " + ssid + ", Set password: " + password + ", Set SSID AP: " + ssidAP + ", Set AP password: " + passwordAP + ", SSDP: " + SSDP_Name);
    HTTP.send(200, "text/plain", "OK");   
}

void handle_time_zone() {     
    timezone = HTTP.arg("timezone").toInt(); 
    isDayLightSaving = HTTP.arg("isDayLightSaving").toInt();
    saveConfig();
    if (typeSync == 1) { //NTP
        myESPTime.setTimeParam(false, timezone, isDayLightSaving, sNtpServerName);
        myESPTime.timeSynch();
    }
#if USE_GPS == true
    if (typeSync == 2) {//GPS
        timeSynchGPS(0, 0);
    }
#endif
    Serial.println("NTP Time Zone: " + String(timezone) + ",  isDayLightSaving: " + String(isDayLightSaving));
    HTTP.send(200, "text/plain", "OK");
}

void handle_ntp_server() {   
    uint8_t oldtypeSync = typeSync;           
    sNtpServerName = HTTP.arg("ntpserver").c_str(); 
    typeSync = HTTP.arg("use_sync").toInt();
#if USE_GPS != true
    if (typeSync == 2) {//GPS but no
        typeSync = oldtypeSync;
    }
#endif  
    saveConfig();
    if (typeSync == 1) { //NTP
        //myESPTime.setTimeParam(useRTC, timezone, isDayLightSaving, sNtpServerName);
        //myESPTime.timeSynch();
        myESPTime.begin(timezone, isDayLightSaving, sNtpServerName, sNtpServerName2, sNtpServerName3, false, false);
    }
#if USE_GPS == true
    if (typeSync == 2) {//GPS
        timeSynchGPS(0, 0);
    }
#endif
    Serial.println("sNtpServerName: " + sNtpServerName + ", typeSync: " + typeSync);
    HTTP.send(200, "text/plain", "OK");
}

void handle_Time() {
    if (typeSync == 1) { //NTP
        myESPTime.timeSynch();
    }
#if USE_GPS == true
    if (typeSync == 2) {//GPS
        timeSynchGPS(0, 0);
    }
#endif
    HTTP.send(200, "text/plain", "OK"); 
}

void handle_TimeNew() {
    struct tm timeman;
    time_t epoch_time;
    uint8_t new_h = HTTP.arg("time_h").toInt(); 
    uint8_t new_m = HTTP.arg("time_m").toInt();
    uint8_t new_s = HTTP.arg("time_s").toInt();
    uint8_t new_day = HTTP.arg("date_day").toInt(); 
    uint8_t new_month = HTTP.arg("date_month").toInt();
    uint16_t new_year = HTTP.arg("date_year").toInt();    
    timeman.tm_hour = new_h; 
    timeman.tm_min = new_m;
    timeman.tm_sec = new_s;
    timeman.tm_mday = new_day;
    timeman.tm_mon = new_month - 1;
    timeman.tm_year = new_year - 1900; 
    epoch_time = mktime(&timeman);// - timezone * 3600;
    struct timeval epoch;
    epoch = {epoch_time, 0};
    settimeofday(&epoch, nullptr);
    printf("Setting time: %s", asctime(&timeman)); 
    Serial.print(F("Setting time UNIX: ")); Serial.println(epoch_time);   
    typeSync = 0;
    //myESPTime.setTimeParam(useRTC, timezone, isDayLightSaving, sNtpServerName);
    //myESPTime.setTimeRTC(epoch_time);
    saveConfig();
    //myESPTime.timeSynch();
    //sMode = 9;
    HTTP.send(200, "text/plain", "OK"); 
}

void handle_set_ledStrip() {
    global_start = HTTP.arg("global_start").toFloat(); global_stop = HTTP.arg("global_stop").toFloat(); 
    brightd = HTTP.arg("brightd").toInt();
    brightn = HTTP.arg("brightn").toInt();
    HTTP.arg("brauto").toInt()==1?brauto=true:brauto=false;
    dmodefrom = HTTP.arg("dmodefrom").toInt();
    dmodeto = HTTP.arg("dmodeto").toInt();
    ledStripMode = HTTP.arg("ledStripMode").toInt();
    HTTP.arg("rndledStripMode").toInt()==1?rndledStripMode=true:rndledStripMode=false;
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
    Serial.print("HTTP ledStripMode ");Serial.println(ledStripMode);
}

void handle_Restart() {
  String restart = HTTP.arg("device");       
  if (restart == "ok") {                      
    HTTP.send(200, "text/plain", "Reset OK");
    ESP.restart();                               
  }
  else {                                       
    HTTP.send(200, "text/plain", "No Reset"); 
  }
}

void handle_resetConfig() {
  String restart = HTTP.arg("device");
  if(restart == "ok") {
    //SPIFFS.format();
    LittleFS.remove("/myconfig.json");
    Serial.println("ESP erase Config file");
    delay(3000);
    HTTP.send(200, "text/plain", "OK");
    delay(3000);
    ESP.restart();
  }
}

bool handleFileRead(String path) {
    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
        if (LittleFS.exists(pathWithGz))
        path += ".gz";
        File file = LittleFS.open(path, "r");
        size_t sent = HTTP.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

String getContentType(String filename) {
    if (HTTP.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".json")) return "application/json";
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

void init_SSDP(void) {
    HTTP.on("/description.xml", HTTP_GET, []() {
        SSDP.schema(HTTP.client());
    });
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName(SSDP_Name);
    SSDP.setSerialNumber("001788102201");
    SSDP.setURL("/");
    SSDP.setModelName(nName);
    SSDP.setModelNumber(nVersion);
    SSDP.setModelURL("http://" + (WiFi.status() != WL_CONNECTED ? WiFi.softAPIP().toString() : WiFi.localIP().toString()));
    SSDP.setManufacturer("LIGHTWELL");
    SSDP.setManufacturerURL("https://led-lightwell.eu");
    SSDP.begin();
}