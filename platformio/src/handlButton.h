void myClickFunction() {
    Serial.println(F("myClickFunction"));
    if (rndledStripMode) { //random mode off
        ledStripMode = 0; 
        rndledStripMode = false;
    }
    ledStripMode++;
    if (ledStripMode > MAX_LED_EFFECTS) ledStripMode = 1;
}

void myDoubleClick() {
    Serial.println(F("myDoubleClick"));
    ledStripStatus = !ledStripStatus;
}

void myPressFunction() {
    Serial.println(F("myPressFunction"));
}

void myPressStart() {
    Serial.println(F("myPressStart"));        
}

void myDuringLongPress() {
    static uint32_t dalayChange = 0;
    if (millis() - dalayChange > 250) {
        //Serial.println(F("myDuringLongPress"));
        if (brightUpDown) {
            if (brightd > 250) brightd = 255; else brightd = brightd +5;
        }
        else {
            if (brightd < 6) brightd = 1; else brightd = brightd -5;
        }
        brightn = brightd;
        LEDS.setBrightness(brightd);
        dalayChange = millis();
        Serial.print(F("bright: ")); Serial.println(brightd);
    }   
}

void myPressStop() {
    Serial.println(F("myPressStop"));
    brightUpDown = !brightUpDown;
    saveConfig();
    brightUpDown ? Serial.println(F("brightUpDown ON")) : Serial.println(F("brightUpDown OFF"));
}