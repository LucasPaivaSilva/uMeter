#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define CurrentPin A0
#define VoltagePin A1

float voltage = 0;
float current = 0;
float power = 0;
int averageCounter = 0;
unsigned long timeControl = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}


void updateDisplay(float v, float c, float p) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(0,0);
  display.println(F(" Power Meter - V1.0  "));
  display.println(F(""));
  display.setCursor(30, 10);
  display.setTextSize(2);                     // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.print(int(abs(v)));
  display.setCursor(80, 10);
  display.print(F("V "));


  display.setCursor(30, 30);
  display.print(abs(c));
  display.setCursor(80, 30);
  display.println(F("A"));

  display.setCursor(30, 50);
  display.print(int(abs(p)));
  display.setCursor(80, 50);
  display.println(F("VA"));

  display.display();
}

float getVoltage(){
  float voltage =  analogRead(VoltagePin) * (5 / 1024.0);
  return voltage * 106;
}

float getCurrent(){
  return 10*((analogRead(CurrentPin) * (5 / 1024.0)) - 2.49);
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("SSD1306 library test"));

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  testdrawrect();      // Draw rectangles (outlines)


}

void loop() {
  voltage += getVoltage();
  current += getCurrent();
  averageCounter++;
  if ((millis() - timeControl) > 100) {
    voltage = voltage / averageCounter;
    current = current / averageCounter;
    power = voltage * current;
    updateDisplay(voltage, current, power);
    averageCounter = 0;
    voltage = 0;
    current = 0;
    timeControl = millis();
  }
}

