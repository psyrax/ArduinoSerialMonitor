#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 12
SSD1306 display(OLED_RESET);

long lastDebounceTime = 0;
long debounceDelay = 3000; 

const int pinLed = 13;
const int pinButton = A0;

String inputString = "";
boolean stringComplete = false;  

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);
  Serial.begin(9600);
  inputString.reserve(200);
  Consumer.begin();
  display.setCursor(0,0);
  display.println("CPU: 20c 100%");
  display.setCursor(0,10);
  display.println("GPU: 20c 100%");
  display.setCursor(0,20);
  display.println("RAM: 50%");
  display.setCursor(0,30);
  display.println("NET: 120M/120M");
  display.drawFastVLine(90, 0, 40, WHITE);

  


  //PAUSE
  //display.fillRect(100, 4, 8, 30, WHITE);
  //display.fillRect(114, 4, 8, 30, WHITE);
  

   
  display.drawFastHLine(0, 40, 128, WHITE);
  display.setCursor(0,42);
  display.println("[INSERT TRACK INFO]");
  display.display();
}

void loop() {

  serialEvent();

  if (stringComplete) {
     display.setTextSize(1);
     display.clearDisplay();
     display.setCursor(0,0);
     display.println(inputString);
     display.display();
     inputString = "";
     stringComplete = false;
  }
  
  int c = analogRead(pinButton);
  if ( c > 0  ) {
    display.fillRect(94, 0, 34, 36, BLACK);
    digitalWrite(pinLed, HIGH);
    Serial.println(c);
    if( c >= 160){
      Consumer.write(MEDIA_PREVIOUS);
      //PREV
      display.fillTriangle(94, 19 ,124, 4,124, 34, WHITE);
      display.fillRect(94, 4, 4, 30, WHITE);
    } else if (c >= 140 && c <= 150){
      Consumer.write(MEDIA_PLAY_PAUSE);
      //PLAY
      display.fillTriangle(94, 34 ,94, 4,124, 19, WHITE);
    } else if ( c >= 110 && c <= 120 ){
      Consumer.write(MEDIA_NEXT);
      //NEXT
      display.fillTriangle(94, 34 ,94, 4,124, 19, WHITE);
      display.fillRect(121, 4, 4, 30, WHITE);
    } else if (  c >= 90 && c <= 100 ){
      Consumer.write(MEDIA_VOLUME_MUTE);
      //SQ
      display.fillRect(94, 4, 30, 30, WHITE);
    } else if (   c >= 60 && c <= 70 ){
      Consumer.write(MEDIA_VOLUME_DOWN);
      //MINUS
      display.fillRect(94, 15, 30, 8, WHITE);
    } else if (  c >= 30 && c <= 40 ){
      Consumer.write(MEDIA_VOLUME_UP);
      //PLUS
      display.fillRect(94, 15, 30, 8, WHITE);
      display.fillRect(105, 4, 8, 30, WHITE);
    };
    display.display();
    delay(150);
    lastDebounceTime = millis();
    digitalWrite(pinLed, LOW);
  }
  /*if ((millis() - lastDebounceTime) > debounceDelay) {
    antiBurn();
  }*/
}
void writeCommand(String printCommand){
  display.setTextSize(3);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(printCommand);
  display.display();
}
void antiBurn(){
  display.clearDisplay();
  display.display();
}
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
