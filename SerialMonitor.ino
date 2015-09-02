#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 12
SSD1306 display(OLED_RESET);

long lastDebounceTime = 0;
long debounceDelay = 3000;
long invertDelay = 300000;
long lastInvertTime = 0;
int invertedStatus = 0;

const int pinLed = 13;
const int pinButton = A0;
const int sButton = A1;
const int pinLigths = 10;
String inputString = "";
boolean stringComplete = false;  

void setup() {

  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();

  display.setTextColor(WHITE);
  pinMode(pinLed, OUTPUT);
  
  pinMode(pinLigths, OUTPUT);
  digitalWrite(pinLigths, LOW);

  pinMode(pinButton, INPUT);
  Serial.begin(9600);
  inputString.reserve(200);
  Consumer.begin();
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

}

void loop() {

  if (lastInvertTime < 1 ){
    lastInvertTime = millis();
  }

  serialEvent();
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
  int s = analogRead(sButton);
  if ( s > 0 ){
    Serial.println(s);
    if ( s >= 60 && s <= 70 ){
      int val = digitalRead(pinLigths);
      Serial.println(val);
      if ( val > 0 ){
        digitalWrite(pinLigths, LOW);
      } else {
        digitalWrite(pinLigths, HIGH);  
      }  
    }
    delay(150);
  }
  int c = analogRead(pinButton);
  if ( c > 0  ) {
    display.fillRect(94, 0, 34, 36, BLACK);
    digitalWrite(pinLed, LOW);
    Serial.println(c);
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
      //SQ
      display.fillRect(94, 4, 30, 30, WHITE);
    } else if (   c >= 140 && c <= 145 ){
      Consumer.write(MEDIA_VOLUME_DOWN);
      //MINUS
      display.fillRect(94, 15, 30, 8, WHITE);
    } else if (  c >= 165 && c <= 170 ){
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
  if ((millis() - lastDebounceTime) > debounceDelay) {
    antiBurn();
  }
  if ((millis() - lastInvertTime) > invertDelay) {
    lastInvertTime = millis();
    inverter();
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
  display.fillRect(94, 0, 34, 36, BLACK);
  display.display();
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

