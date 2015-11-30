#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

//#define USE_TEENSY_KEYBOARD
#include "HID-Project.h"

// OLED setup
#define OLED_RESET 12
SSD1306 display(OLED_RESET);
int oledDraw = 0;
int oledOverride = 0;

// Neopixel setup
#define PIN 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// Debpunce timers for buttons
long lastDebounceTime = 0;
long debounceDelay = 3000;

// Inverted timers for oled
long invertDelay = 300000;
long lastInvertTime = 0;
int invertedStatus = 0;

// media buttons
const int pinButton = A0;

//extra butons
const int sButton = A1;

//tray button
const int tButton = A2;

// vars for serial input
String inputString = "";
boolean stringComplete = false;

// initial neopixel status
int lightStatus = 0;
int lightOverride = 0;
int loginStatus = 1;
void setup() {
  /* NEO PIXEL SETUP */
  strip.begin();
  strip.show();

  /* PIN SETUP */
  pinMode(pinButton, INPUT);
  pinMode(sButton, INPUT);
  
  
  // Tray button 
  pinMode(tButton, INPUT);
 

  /* OLED SETUP */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextColor(WHITE);
  oledDrawBackground();
  
  
  /* Media keys setup */
  Consumer.begin();
  /* Teensy for mod shortcuts */
  //TeensyKeyboard.begin();
  /* Keyboard FTW */
  Keyboard.begin();
 

  /* Serial setup */
  Serial.begin(9600);
  inputString.reserve(200);

}

void loop() {

  
  if (lastInvertTime < 1 ){
    lastInvertTime = millis();
  }

  serialEvent();

  /* OLED DRAW STATS */
  if (stringComplete) {
     display.setTextSize(1);
     display.fillRect(25, 0, 64, 39, BLACK);
     display.setCursor(25,0);
     int cpuStringStart = inputString.indexOf("C");
     int cpuStringLimit = inputString.indexOf("|");
     String cpuString = inputString.substring(cpuStringStart+1, cpuStringLimit);
     display.println(cpuString);
     display.setCursor(25,10);
     int gpuStringStart = inputString.indexOf("G", cpuStringLimit);
     int gpuStringLimit = inputString.indexOf("|", gpuStringStart);
     String gpuString = inputString.substring(gpuStringStart+1 ,gpuStringLimit);
     display.println(gpuString);
     display.setCursor(25,20);
     int ramStringStart = inputString.indexOf("R", gpuStringLimit);
     int ramStringLimit = inputString.indexOf("|", ramStringStart);
     String ramString = inputString.substring(ramStringStart+1 ,ramStringLimit);
     display.println(ramString);
     display.fillRect(0, 42, 128, 22, BLACK);
     display.setCursor(0,42);
     int songStringStart = inputString.indexOf("S", ramStringLimit);
     int songStringLimit = inputString.indexOf("|", songStringStart);
     String songString = inputString.substring(songStringStart+1, songStringLimit);
     display.println(songString);
     display.display();
     inputString = "";
     stringComplete = false;
  }


  /* MEDIA EVENTS */
  int c = analogRead(pinButton);
  if ( c > 50  ) {
    display.fillRect(94, 0, 34, 36, BLACK);
    if( c >= 510){
      Consumer.write(MEDIA_PREVIOUS);
      //PREV
      display.fillTriangle(94, 19 ,124, 4,124, 34, WHITE);
      display.fillRect(94, 4, 4, 30, WHITE);
    } else if (c >= 60 && c <= 65){
      Consumer.write(MEDIA_PLAY_PAUSE);
      //PLAY
      display.fillTriangle(94, 34 ,94, 4,124, 19, WHITE);
    } else if ( c >= 89 && c <= 95 ){
      Consumer.write(MEDIA_NEXT);
      //NEXT
      display.fillTriangle(94, 34 ,94, 4,124, 19, WHITE);
      display.fillRect(121, 4, 4, 30, WHITE);
    } else if (  c >= 116 && c <= 120 ){
      Consumer.write(MEDIA_VOLUME_MUTE);
      //MUTE
      display.fillRect(94, 4, 30, 30, WHITE);
    } else if (   c >= 140 && c <= 145 ){
      Consumer.write(MEDIA_VOLUME_DOWN);
      //VOL DOWN
      display.fillRect(94, 15, 30, 8, WHITE);
    } else if (  c >= 165 && c <= 170 ){
      Consumer.write(MEDIA_VOLUME_UP);
      //VOL UP
      display.fillRect(94, 15, 30, 8, WHITE);
      display.fillRect(105, 4, 8, 30, WHITE);
    };
    if ( oledDraw == 1 ){
      display.display();
    }
    delay(50);
    lastDebounceTime = millis();
  }
  /* CLEAR MEDIA SQUARE */
  if ((millis() - lastDebounceTime) > debounceDelay) {
    clearMedia();
  }
  /* SCREEN INVERT */
  if ((millis() - lastInvertTime) > invertDelay && oledDraw == 1) {
    lastInvertTime = millis();
    inverter();
  }

  
  int s = analogRead(sButton);
  if ( s > 25 ){
    if ( s >= 28 && s <= 50 ){
      if ( oledOverride == 0 ){
        oledOverride = 1;
      } else if ( oledOverride == 1 ){
        oledOverride = 0;
      }
      if ( oledDraw == 1 ){
        antiBurn(); 
      } else if ( oledDraw == 0 ){
        oledDrawBackground();
      }
    }
    if ( s >= 60 && s <= 70 ){
      if ( lightOverride == 0 ){
        lightOverride = 1;
      } else if ( lightOverride == 1 ){
        lightOverride = 0;
      }
      if( lightStatus==0 && lightOverride == 1 ){
        lightStatus = 1;
        colorWipe(strip.Color(0,255,0), 0);
      } else if ( lightStatus == 1 &&  lightOverride == 1 ){
        lightStatus = 0;
        colorWipe(strip.Color(0,0,0), 0); 
      }
    }
    delay(50);
  }

  /* TRAY BUTTON */
  int t = analogRead(tButton);

  if ( t>5 && loginStatus == 1){
    loginStatus = 0;
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press('l');
    Keyboard.releaseAll();
    delay(300);
  } else if ( t<5 && loginStatus == 0){
    loginStatus = 1;
    Keyboard.write(KEY_ESC);
    delay(2000);
    Keyboard.println("PASSWORD HERE");
    delay(300);
  }

  /* Leds off */
  if ( t>5 && lightOverride == 0 ){
     lightStatus = 0;
     colorWipe(strip.Color(0,0,0), 0); 
  } else if ( t<5 &&  lightStatus == 0 && lightOverride == 0 ){
    lightStatus = 1;
    colorWipe(strip.Color(0,255,0), 5);
  }
  /* oled off */
  if ( t>5 && oledDraw == 1 ){
     antiBurn();
  } else if ( t<5 &&  oledDraw == 0 && oledOverride == 0  ){
    oledDrawBackground();
  }
  
}
void writeCommand(String printCommand){
  display.setTextSize(3);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(printCommand);
  display.display();
}
void antiBurn(){
  display.invertDisplay(0);
  display.fillRect(0, 0, 128, 64, BLACK);
  display.display();
  oledDraw = 0;
}
void clearMedia(){
  display.fillRect(94, 0, 34, 36, BLACK);
  display.display();
}
void oledDrawBackground(){
  display.setCursor(0,0);
  display.println("CPU:");
  display.setCursor(0,10);
  display.println("GPU:");
  display.setCursor(0,20);
  display.println("RAM:");
  display.setCursor(0,30);
  //display.println("NET:");
  display.drawFastVLine(90, 0, 40, WHITE);
  display.drawFastHLine(0, 40, 128, WHITE);
  display.display();
  oledDraw = 1;
}
void inverter(){
  if ( invertedStatus == 1 ){
    invertedStatus = 0;
  } else {
    invertedStatus = 1;
  };
  display.invertDisplay(invertedStatus);
  display.display();
}
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '|') {
      stringComplete = true;
    }
  }
}



// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


