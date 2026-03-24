#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "RTC.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo myServo;

const int buttonPin = 2;   
const int servoPin = 6;    
const int ledPin = 8;      
const int buzzerPin = 12;  

int angle = 0;              
int angleIncrement = 45;    
int movementDelay = 50;     

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  myServo.attach(servoPin);
  myServo.write(angle); 
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting");

  RTC.begin();
  RTCTime startTime(18, Month::FEBRUARY, 2026, 8, 59, 50, DayOfWeek::WEDNESDAY, SaveLight::SAVING_TIME_INACTIVE);
  RTC.setTime(startTime);
  
  delay(1000);
  lcd.clear();
}

void loop() {
  RTCTime currentTime;
  RTC.getTime(currentTime);
  
  updateLCD(currentTime);

  if (currentTime.getSeconds() % 10 == 0) {
    dispensePill();
    delay(1000); 
  }
}

void dispensePill() {
  int targetAngle = angle + angleIncrement;
  
  if (targetAngle <= 180) {
    while (angle < targetAngle) {
      angle++;
      myServo.write(angle);
      delay(movementDelay);
    }
  } else {
    while (angle > 0) {
      angle--;
      myServo.write(angle);
      delay(movementDelay);
    }
    angle = 0;
  }

  triggerAlarm();
}

void triggerAlarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Take Medicine!");

  while (digitalRead(buttonPin) == HIGH) {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 1000);
    delay(150);
    
    digitalWrite(ledPin, LOW);
    noTone(buzzerPin);
    delay(150);
  }
  
  lcd.clear();
  lcd.print("Thank You!");
  delay(2000);
  lcd.clear();
}

void updateLCD(RTCTime t) {
  lcd.setCursor(0, 0);
  if((int)t.getMonth() + 1 < 10) lcd.print("0"); 
  lcd.print((int)t.getMonth() + 1); 
  lcd.print("/");
  if(t.getDayOfMonth() < 10) lcd.print("0");
  lcd.print(t.getDayOfMonth());
  lcd.print("/");
  lcd.print(t.getYear());

  lcd.setCursor(0, 1);
  if(t.getHour() < 10) lcd.print("0");
  lcd.print(t.getHour());
  lcd.print(":");
  if(t.getMinutes() < 10) lcd.print("0");
  lcd.print(t.getMinutes());
  lcd.print(":");
  if(t.getSeconds() < 10) lcd.print("0");
  lcd.print(t.getSeconds());
}