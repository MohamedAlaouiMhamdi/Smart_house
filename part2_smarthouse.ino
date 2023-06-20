/*
Adafruit Arduino - Lesson 13. DC Motor
*/
#include "DHT.h"
 int water;
#define bazzer 7
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int motorPin = 3;
 int capteur_lum = 0; // capteur branché sur le port 0
int analog_lum; // valeur analogique envoyé par le capteur
#define green  12
void setup() 
{ 
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  pinMode(green, OUTPUT); // démarrer la liaison série
 
  // Initialise la capteur DHT11
  dht.begin();
} 
 
 
void loop() 
{ 
   
       Serial.println(water);
       delay(3000);
  readwaterSensor();
  
     dht_fan();
     capteur_lumiere();


  
} 



void dht_fan(){
  
  Serial.println("Temperature = " + String(dht.readTemperature())+" °C");
       Serial.println("Humidite = " + String(dht.readHumidity())+" %");
         
 if(dht.readTemperature()>20){


    
      digitalWrite(motorPin,HIGH);
      delay(3000);
      
      
       
      }
      else{
        digitalWrite(motorPin,LOW);
     
      

      }
        
delay(1000);
}

void capteur_lumiere(){

  analog_lum = analogRead(capteur_lum); // lecture de la valeur analogique, qu'on enregistre dans analog_lum
  Serial.print("Valeur luminosité = ");
  Serial.print(analog_lum); 
  Serial.println("");
  delay(100);
  if(analog_lum>238){
     digitalWrite(green, LOW);
     delay(100);
    
  }
    else{
     
      digitalWrite(green, HIGH); 
     delay(100);
       }
}

void readwaterSensor(){
       water = analogRead(A1);
       if (water>350 ){
           
           tone (bazzer, 300); // allume le buzzer actif arduino
           delay(500);
       }
      else {  
        
        noTone(bazzer);
        delay(500);
       }
}
