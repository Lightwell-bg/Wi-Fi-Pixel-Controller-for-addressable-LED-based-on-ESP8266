#include <Arduino.h>
#include "config.h"
#include "configFile.h"
#include "fwareupd.h"
#include "httpServer.h"
#include "ledEffects.h"
#include "ledStrip.h"
#include "handlButton.h"
#include "webSocket.h"


void setup() {
  Serial.begin(115200);
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  buttonControl.setDebounceTicks(50);
  buttonControl.attachClick(myClickFunction);
  buttonControl.setPressTicks(1500); // that is the time when LongPressStart is called
  buttonControl.attachPress(myPressFunction);
  buttonControl.attachDoubleClick(myDoubleClick);
  buttonControl.attachLongPressStart(myPressStart);
  buttonControl.attachLongPressStop(myPressStop);
  buttonControl.attachDuringLongPress(myDuringLongPress);
  char host[16];
  if(!LittleFS.begin()) { // Initialize SPIFFS
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
  } 
  loadConfig(); Serial.println(F("FileConfig init"));
  myWIFI.begin(ssid.c_str(), password.c_str(), SSDP_Name.c_str(), ssidAP.c_str(), passwordAP.c_str(), apIP.c_str()); Serial.println(F("Wi-Fi init"));
  if (typeSync == 1) { //if use GPS we cant set timezone on ESP
        myESPTime.begin(timezone, isDayLightSaving, sNtpServerName, sNtpServerName2, sNtpServerName3, false, false);
        Serial.println(F("Start Time"));
    }
    init_firmWareUpdate(); Serial.println(F("Start init FirmWare update"));
    SSDP_Name = myWIFI.getNameSSDP(); Serial.println(SSDP_Name);
    if (WiFi.status() == WL_CONNECTED)  {
        init_SSDP(); Serial.println(F("Start SSDP"));   //Run SSDP
    }
    else {
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError); // Captive_Portal
        dnsServer.start(DNS_PORT, F("*"), WiFi.softAPIP()); // Captive_Portal
    }
    init_HTTPServer(); Serial.println(F("HTTPServer init"));
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    SSDP_Name.toCharArray(host, SSDP_Name.length()+1);
    NBNS.begin(SSDP_Name.c_str());
    Serial.printf("Ready! Open http://%s in your browser\n", host);
    Serial.println(myWIFI.getDevStatusIP()); 
    myWIFI.beginUDP(localUdpPort); //init_udp();
    initLedStrip();
    delay(500);
    randomSeed(micros());
}

void loop() {
    static uint32_t curCount = 0;
    static uint8_t oldledStripMode = 254;
    dnsServer.processNextRequest(); // Captive_Portal
    HTTP.handleClient();
    myWIFI.returnIPtoUDP(); //listenUDP();
    webSocket.loop();
    buttonControl.tick();
    struct tm tnow = myESPTime.getTimeStruct();
    if (myESPTime.compTimeInt(global_start, global_stop, &tnow) && ledStripStatus) {
        if (myESPTime.compTimeInt(dmodefrom, dmodeto, &tnow)) LEDS.setBrightness(brightd); else LEDS.setBrightness(brightn);
        if (rndledStripMode) {
            if ((millis() - curCount) > MAX_LED_EFFECTS_TIME * 1000) {
                ledStripMode = random(1, MAX_LED_EFFECTS + 1);
                if(ledStripMode == 20) ledStripMode = 1;//stop if 20 ? see later 
                curCount = millis();
            }
        }
        if (oldledStripMode != ledStripMode) {
            Serial.print("New ledStripMode ");Serial.println(ledStripMode);
            oldledStripMode = ledStripMode;
        }
        ledStripShow(ledStripMode);
    }
    else {
        LEDS.clearData();
        LEDS.clear();
        LEDS.show();
    }

    ESP.wdtFeed();
    yield();
}