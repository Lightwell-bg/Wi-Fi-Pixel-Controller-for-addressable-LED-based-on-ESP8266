//функция обработки входящих сообщений
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
   if(type == WStype_CONNECTED){
      IPAddress ip = webSocket.remoteIP(num);
      String message = String("Connected");
      webSocket.broadcastTXT(message);   // отправляем последнее значение всем клиентам при подключении    
    }
    
    if(type == WStype_TEXT){
        String data;
        for(int x = 0; x < length; x++){
          if(!isdigit(payload[x])) continue;
          data += (char) payload[x];          
        }
        
        if(payload[0] == 'B'){
          flag = 0;
          Serial.print("Bright: ");
          brightd = data.toInt();
          Serial.println(data);
          LEDS.setBrightness(brightd);
        }  
        else if(payload[0] == 'F'){
          flag = 0;
          Serial.print("Function: ");
          ledStripMode = data.toInt();
          Serial.println(data);
          ledStripShow(ledStripMode);
        }
        else if(payload[0] == '#') {  
          if(!flag){
              Serial.print("flag : ");
              Serial.println(flag);
              ledStripMode = flag;
              ledStripShow(ledStripMode);
              flag = 1;
          }
          else {
           //преобразуем в 24 битное цветовое число
           uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);         
           //преобразуем 24 бит по 8 бит на канал 
           /*uint8_t r = abs(0 + (rgb >> 16) & 0xFF);
           uint8_t g = abs(0 + (rgb >>  8) & 0xFF);
           uint8_t b = abs(0 + (rgb >>  0) & 0xFF);*/
           uint8_t r = (rgb >> 16) & 0xFF;
           uint8_t g = (rgb >>  8) & 0xFF;
           uint8_t b = (rgb >>  0) & 0xFF;           
           Serial.print("ColorPicker: ");
           Serial.print(r);
           Serial.print(g);
           Serial.println(b);
           
           for(int x = 0; x < LED_COUNT; x++){
             leds[x].setRGB(r,g,b);
           }
           LEDS.show();            
          }
       }
   } 
}