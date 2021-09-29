#ifndef GLOBAL_H
#define GLOBAL_H

const String nName = "LWStrip_"; 
const String nVersion = "v1.0";
#define USE_RTC false //USE RTC chip DS3231 
#define TEST_MODE false //display adv message when test mode

#include <ESP8266WiFi.h>   
#include <ESP8266WebServer.h> 
#include <ESP8266NetBIOS.h>
#include <ESP8266HTTPClient.h>  
#include <ESP8266SSDP.h> 
#include <Updater.h>//for update firmware and SPIFFS
#include <WiFiUdp.h>
#include <DNSServer.h> // Captive_Portal
#include <LittleFS.h>
#include "StreamString.h" //need FOR OUTPUT ERROR when update firmware
#include <ArduinoJson.h>        //https://github.com/bblanchon/ArduinoJson.git 
#include <TimeLib.h>            //https://github.com/PaulStoffregen/Time Michael Margolis
#include <Timezone.h>           //https://github.com/JChristensen/Timezone
#include "NetCrtESP.h"
#include "ESPTimeFunc.h"
#include <FastLED.h>
#include <WebSocketsServer.h>
#include "OneButton.h"

//For dispalay name of device
#if USE_RTC == true
  const String rtsN = " +RTC";
#else  
  const String rtsN = " no RTC";
#endif
String cVersion = nName + nVersion + rtsN;

#define PIN_BUTTON D6 //Control button

ESP8266WebServer HTTP; 
const uint8_t DNS_PORT = 53;
DNSServer dnsServer;
File fsUploadFile; 
WiFiClient ESPclient;
WiFiUDP Udp;
NetCrtESP myWIFI;
ESPTimeFunc myESPTime(false);
OneButton buttonControl(PIN_BUTTON, true);
WebSocketsServer webSocket(81); // инициализация websocket на 81 порту

String filePath = "/myconfig.json"; //File for config
String jsonConfig = "{}";

// wifi
String ssid = "";
String password = "";
String ssidAP = "LWStrip01";   // SSID AP 
String passwordAP = ""; // password AP
String SSDP_Name = "LWStrip01"; // SSDP Name
String apIP = "192.168.4.1";
//UDP
uint16_t localUdpPort = 4210;
char incomingPacket[255];
//char replyPacket[] = "Hi there! Got the message :-)";

uint8_t lang = 2; //0-RU, 1-BG, 2 -EN, 
//Time
uint8_t typeSync = 1; //type of synchronuzation 0 - manual, 1 - NTP, 2 - GPS, 
int8_t timezone = 2;               // часовой пояс GTM
bool isDayLightSaving = true; //Summer time use
String sNtpServerName = "us.pool.ntp.org";
const char sNtpServerName2[] = "pool.ntp.org";
const char sNtpServerName3[] = "time.nist.gov";
TimeChangeRule localCEST = {"CEST", Last, Sun, Mar, 2, 180};     // LOCAL Summer Time
TimeChangeRule localCET = {"CET ", Last, Sun, Oct, 3, 120};       // LOCAL Standard Time
Timezone localCE(localCEST, localCET);
const uint32_t PERIOD_TIME_SEND = 1000*60*90;

//Setup for LED
bool brauto = false;
uint8_t dmodefrom = 8, dmodeto = 20; //DAY MODE
uint8_t brightd = 255, brightn = 85; //brightness day and night
uint16_t speedTicker = 5; // speed of creeping line
//SEtup for show
float global_start = 0, global_stop = 24; //Working time

//LED STRIP
#define LED_COUNT 50 // NUMBER of LEDs
#define LED_DT D5    // Control pin for LEDs

#define MAX_LED_EFFECTS  29
#define MAX_LED_EFFECTS_TIME  5 //sec
uint8_t ledStripMode = 1; // эффект (0 - 29)
bool rndledStripMode = true;
bool ledStripStatus = true; //ON OFF by buttom
bool brightUpDown = true; //for Button myPressFunction
uint8_t flag = 1; // флаг отмены эффекта
CRGBArray<LED_COUNT> leds;
uint8_t delayValue = 20; // задержка
uint8_t stepValue = 10; // шаг по пикселям
uint8_t hueValue = 0; // тон цвета
//LED Effects
uint8_t idex = 0; //индекс текущего пикселя    
uint8_t ihue = 0; // тон цвета
uint8_t saturationVal = 255; // насыщенность 
uint8_t ibright = 0; //значение яркости
uint16_t TOP_INDEX = uint8_t(LED_COUNT / 2); // получаем середину ленты
uint8_t EVENODD = LED_COUNT % 2; //флаг проверки четности
uint8_t bouncedirection = 0; //флаг для color_bounce()
int ledsX[LED_COUNT][3]; //массив для сохранения случайных значений пикселя 
#endif