void initLedStrip() {
    LEDS.setBrightness(brightd);
    LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // config LEDs type (WS2811, WS2812, WS2812B)
    updateColor(0,0,0);
    LEDS.show(); 
}

void ledStripShow(uint8_t ledMode){ 
    switch(ledMode){
      case 0: updateColor(0,0,0); break;
      case 1: rainbow_fade(); delayValue = 20; break;       
      case 2: rainbow_loop(); delayValue = 20; break;
      case 3: new_rainbow_loop(); delayValue = 5; break;
      case 4: random_march(); delayValue = 40; break;  
      case 5: rgb_propeller(); delayValue = 25; break;
      case 6: rotatingRedBlue(); delayValue = 40; hueValue = 0; break;
      case 7: Fire(55, 120, delayValue); delayValue = 15; break; 
      case 8: blueFire(55, 250, delayValue); delayValue = 15; break;  
      case 9: random_burst(); delayValue = 20; break;
      case 10: flicker(); delayValue = 20; break;
      case 11: random_color_pop(); delayValue = 35; break;                                      
      case 12: Sparkle(255, 255, 255, delayValue); delayValue = 0; break;                   
      case 13: color_bounce(); delayValue = 20; hueValue = 0; break;
      case 14: color_bounceFADE(); delayValue = 40; hueValue = 0; break;
      case 15: red_blue_bounce(); delayValue = 40; hueValue = 0; break;
      case 16: rainbow_vertical(); delayValue = 50; stepValue = 15; break;
      case 17: matrix(); delayValue = 50; hueValue = 95; break; 
      // тяжелые эффекты
      case 18: rwb_march(); delayValue = 80; break;                         
      case 19: flame(); break;
      case 20: theaterChase(255, 0, 0, delayValue); delayValue = 50; break;
      case 21: Strobe(255, 255, 255, 10, delayValue, 1000); delayValue = 100; break;
      case 22: policeBlinker(); delayValue = 25; break;
      case 23: kitt(); delayValue = 100; break;
      case 24: rule30(); delayValue = 100; break;
      case 25: fade_vertical(); delayValue = 60; hueValue = 180; break;
      case 26: fadeToCenter(); break;
      case 27: runnerChameleon(); break;
      case 28: blende(); break;
      case 29: blende_2();
    }
}