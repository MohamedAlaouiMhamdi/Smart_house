#include <Wire.h> 
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Ultrasonic.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define echoPin 15 // Echo Pin
#define trigPin 16 // Trigger Pin
long duration, distance;
#define ROW_NUM    4  // four rows
#define COLUMN_NUM 4  // four columns
#define SERVO_PIN  14// // the Arduino pin, which connects to the servo motor
#define red 18
#define green  19
#define g 13
Servo servo; // servo motor
#define PIR 12
#include "IRremote.h"
Servo servogar;  // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0; 
IRrecv IR(A1);

int  flame;


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password_1 = "55"; // change your password here
   // change your password here
String input_password;

int angle = 0; // the current angle of servo motor
unsigned long lastTime;

#define bazzer  24// RES pin
#define  flamePin 17

#define RST_PIN  49// RES pin
#define SS_PIN  10 // SDA (SS) pin
byte readCard[4];
String cardID = "ED62232"; // remplacer par l'ID de votre tag
String tagID = "";

MFRC522 module_rfid(SS_PIN, RST_PIN);

long dis;   // Broche ECHO

void setup() {
  IR.enableIRIn(); 
  servogar.attach(44); 
  servogar.write(0);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(PIR, INPUT);
  pinMode(flamePin, INPUT);
  pinMode(bazzer, OUTPUT);
  Serial.begin(9600);
  SPI.begin();
  module_rfid.PCD_Init();
  lcd.begin();
  lcd.backlight();
  servo.attach(SERVO_PIN);
  servo.write(90); // rotate servo motor to 0°
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(g, OUTPUT);
  lastTime = millis();
}

void loop() {
  
  telecmd();
  char key = keypad.getKey();
  if (key) {
    Serial.println(key);
    if (key == '*') {
      input_password = ""; // reset the input password
    } else if (key == '#') {
      if (input_password == password_1 ) {
        correct();
  
      } else {
        incorrect();
  
      }

      input_password = ""; // reset the input password
    } else {
      input_password += key;
  
      for (int i=1;i<=input_password.length();i++){
       lcd.println("*");
      }
      
      delay(4000);
      lcd.clear();
  
    }
  }

  if (angle == -180 && (millis() - lastTime) > 2000) { // 5 seconds
    closedoor();
  }
 

  dis=distancee();
  if( angle<120 && dis<10 )
  {
   displayscreen();
  }
  readFlameSensor();
  movdetcted();
  while (getID()) {
    if (tagID == cardID) {
      Serial.println("Access Granted!");
      Serial.println(tagID);
      correct();
    }
    else {
      incorrectT();
    
    }
    Serial.print("ID: ");
    Serial.println(tagID);
    
  }

  

}




void correct(){
   delay(500);
   lcd.clear();
   lcd.print("WELCOME AGAIN!");
   Serial.println("The password is correct, rotating Servo Motor to 90°");
   digitalWrite(green, HIGH); 
   digitalWrite(red, LOW);
   delay(4000);
   lcd.clear();  
   angle = -180;
   servo.write(angle);
   lastTime = millis();
}

void incorrect(){
    delay(500);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("CODE");
    lcd.setCursor(6,0);
    lcd.print("INCORRECT");
    lcd.setCursor(15,1);
    lcd.println(" ");
    lcd.setCursor(4,1);
    delay(2000);
   
    digitalWrite(red, HIGH); 
    lcd.println("TRY AGAIN !!!");
    delay(4000);
    digitalWrite(red, LOW); 
        lcd.clear();

}
void displayscreen()
{
 
  lcd.setCursor(0,0);
  lcd.println("ENTER THE CODE");
  lcd.setCursor(1 ,1);
  lcd.println("TO OPEN DOOR");
  delay(1000);
  lcd.clear();
}
void closedoor(){
    angle = 90;
    servo.write(angle);
    Serial.println("Rotating Servo Motor to 0°");
    digitalWrite(green, LOW);  
    digitalWrite(red, HIGH); 
    delay(1000) ;
    digitalWrite(red, LOW);
}


boolean getID() {
  if (! module_rfid.PICC_IsNewCardPresent()) {
    return false;
  }

  if (! module_rfid.PICC_ReadCardSerial()) {
    return false;
  }

  tagID = "";
  
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(module_rfid.uid.uidByte[i], HEX));
  }

  tagID.toUpperCase();
  module_rfid.PICC_HaltA();
  return true;
}
void incorrectT(){
    delay(500);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("CART");
    lcd.setCursor(6,0);
    lcd.print("INVALID");
    lcd.setCursor(15,1);
    lcd.println(" ");
    lcd.setCursor(4,1);
    delay(2000);
   
    digitalWrite(red, HIGH); 
    lcd.println("TRY AGAIN !!!");
    delay(4000);
    digitalWrite(red, LOW); 
        lcd.clear();

}
long distancee(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
  return distance;
}
void readFlameSensor(){
 	     flame = digitalRead(flamePin);
       if (flame ==0 ){
           tone (bazzer, 300); // allume le buzzer actif arduino
           delay(500);
       }
      else {  
        noTone(bazzer);
        delay(500);
         }  
} 

void movdetcted(){
     if (digitalRead(PIR) == HIGH) {
       digitalWrite(g, HIGH);}
     if (digitalRead(PIR) == LOW ) {
      digitalWrite(g, LOW);}
}

void telecmd (){

  if (IR.decode()) {

    if (IR.decodedIRData.decodedRawData==0xE718FF00){ 
       servogar.write(180); }
    if (IR.decodedIRData.decodedRawData==0xF30CFF00){ 
       servogar.write(0); }
  IR.resume();
  }
}
