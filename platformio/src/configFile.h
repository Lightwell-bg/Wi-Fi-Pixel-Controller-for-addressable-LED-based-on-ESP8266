bool saveConfig();

bool loadConfig() {
    if(!LittleFS.exists(filePath)) {
        Serial.println(F("Failed to open config file"));
        saveConfig();   
        return false;
    }
    File configFile = LittleFS.open(filePath, "r");   
    size_t size = configFile.size();   
    if (size > 2048) {  
        Serial.print(F("Config file size is too large: ")); Serial.println(size);
        configFile.close();
        return false;
    }
    jsonConfig = configFile.readString(); 
    configFile.close();
    DynamicJsonDocument jsonDoc(5096); //4096
    DeserializationError error = deserializeJson(jsonDoc, jsonConfig);
    if (error) {
      Serial.print(F("loadConfig() deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return false;
    }   
    JsonObject root = jsonDoc.as<JsonObject>();  
    timezone = root["timezone"].as<int>();               
    isDayLightSaving = root["isDayLightSaving"];
    sNtpServerName = root["ntp_server"].as<String>(); 
    typeSync = root["typeSync"];   
    lang = root["lang"].as<int>();
    brightd = root["brightd"]; brightn = root["brightn"];
    brauto = root["brauto"];
    dmodefrom = root["dmodefrom"]; dmodeto = root["dmodeto"];             
    speedTicker = root["speedTicker"]; 
    global_start = root["global_start"]; global_stop = root["global_stop"];
    ledStripMode = root["ledStripMode"];
    if (ledStripMode == 255) rndledStripMode = true; else rndledStripMode = false;
    rndledStripMode = root["rndledStripMode"];
    
    return true;
}

bool saveConfig() {
    DynamicJsonDocument jsonDoc(5096); //4096
    DeserializationError error = deserializeJson(jsonDoc, jsonConfig);
    if (error) {
        Serial.print(F("saveConfig() deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return false;
    }     
    JsonObject json = jsonDoc.as<JsonObject>();  
    json["timezone"] = timezone;
    json["isDayLightSaving"] = isDayLightSaving;
    json["ntp_server"] = sNtpServerName;
    json["typeSync"] = typeSync;
    json["lang"] = lang;
    json["cVersion"] = cVersion;
    json["brightd"] = brightd; json["brightn"] = brightn;
    json["brauto"] = brauto;
    json["dmodefrom"] = dmodefrom; json["dmodeto"] = dmodeto;
    json["global_start"] = global_start; json["global_stop"] = global_stop;
    json["ledStripMode"] = ledStripMode;
    json["rndledStripMode"] = rndledStripMode;
    
    serializeJson(json, jsonConfig);
    File configFile = LittleFS.open(filePath, "w");
    if (!configFile) {
        Serial.println(F("Failed to open config file for writing"));
        configFile.close();
        return false;
    }
    serializeJson(json, configFile);
    configFile.close();
    return true;
}