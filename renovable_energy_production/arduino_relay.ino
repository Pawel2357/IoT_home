#include "EmonLib.h"                   // Include Emon Library

String a; //Initialized variable to store recieved data
const int pin_1 = 2;
const int pin_2 = 3;
const int pin_3 = 4;
const int pin_4 = 5;
const int pin_5 = 6;
const int pin_6 = 7;
const int pin_7 = 8;
const int pin_8 = 9;

EnergyMonitor emon1;                   // Create an instance
EnergyMonitor emon2;                   // Create an instance
EnergyMonitor emon3;                   // Create an instance
EnergyMonitor emon4;                   // Create an instance
unsigned long lastStatus = 0;

boolean inverter_running = false;
boolean kitchen_load_on = false;

void setup() {
  //Serial Begin at 9600 Baud
  pinMode(pin_1, OUTPUT);
  pinMode(pin_2, OUTPUT);
  pinMode(pin_3, OUTPUT);
  pinMode(pin_4, OUTPUT);
  pinMode(pin_5, OUTPUT);
  pinMode(pin_6, OUTPUT);
  pinMode(pin_7, OUTPUT);
  pinMode(pin_8, OUTPUT);
  Serial.begin(9600);
  digitalWrite(pin_1, HIGH);
  digitalWrite(pin_2, HIGH);
  digitalWrite(pin_3, HIGH);
  digitalWrite(pin_4, HIGH);
  digitalWrite(pin_5, HIGH);
  digitalWrite(pin_6, HIGH);
  digitalWrite(pin_7, HIGH);
  digitalWrite(pin_8, HIGH);
  emon1.current(2, 22);             // Current: input pin, calibration.
  emon2.current(3, 22);             // Current: input pin, calibration.
  emon3.current(4, 22);             // Current: kitchen/dishwasher
  emon4.current(5, 22);             // Current: battery
}

void loop() {
  double power_kitchen = emon3.calcIrms(1480) * 230.0;  // Calculate Irms only
  if(power_kitchen > 2150){
    if(kitchen_load_on==true){
      digitalWrite(pin_1, HIGH);
      digitalWrite(pin_2, HIGH);
      digitalWrite(pin_3, HIGH);
      digitalWrite(pin_4, HIGH);
      digitalWrite(pin_5, HIGH);
      digitalWrite(pin_6, HIGH);
      digitalWrite(pin_8, HIGH);
      kitchen_load_on = false;
      delay(100);
      digitalWrite(pin_7, LOW);
      delay(1500);
    }
  }
  

  if (Serial.available() > 0) {

    if (millis() - lastStatus > 1000){
      double Irms = emon1.calcIrms(1480);  // Calculate Irms only
      Serial.print(String(Irms * 230.0) + ",Power_7,");         // Apparent power
    }else if(millis() - lastStatus > 2000){
      double Irms_hp = emon2.calcIrms(1480);  // Calculate Irms only
      Serial.print(String(Irms_hp * 230.0) + ",Power_hp");         // Apparent power
    }else if(millis() - lastStatus > 3000){
      double Irms_kitchen = emon3.calcIrms(1480);  // Calculate Irms only
      Serial.print(String(Irms_kitchen * 230.0) + ",Power_kitchen,");
    }else if(millis() - lastStatus > 4000){
      double Irms_battery = emon4.calcIrms(1480);  // Calculate Irms only
      Serial.print(String(Irms_battery * 230.0) + ",Power_hp");
      lastStatus = millis();
    }
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    if (a == "7" && inverter_running == false){
      digitalWrite(pin_1, HIGH);
      digitalWrite(pin_2, HIGH);
      digitalWrite(pin_3, HIGH);
      digitalWrite(pin_4, HIGH);
      digitalWrite(pin_5, HIGH);
      digitalWrite(pin_6, HIGH);
      digitalWrite(pin_8, HIGH);
      kitchen_load_on = false;
      delay(100);
      digitalWrite(pin_7, LOW);
      delay(1500);
      inverter_running = true;
    }if (a == "f"){
      digitalWrite(pin_1, HIGH);
      digitalWrite(pin_2, HIGH);
      digitalWrite(pin_3, HIGH);
      digitalWrite(pin_4, HIGH);
      digitalWrite(pin_5, HIGH);
      digitalWrite(pin_6, HIGH);
      digitalWrite(pin_8, HIGH);
      kitchen_load_on = false;
      delay(100);
      digitalWrite(pin_7, HIGH);
      delay(900);
      inverter_running = false;
    }
    if (inverter_running == true){
      // say what you got:
      if (a == "1"){
        digitalWrite(pin_1, LOW);
        //Serial.print(a);
        kitchen_load_on = true;
      }if (a == "a"){
        digitalWrite(pin_1, HIGH);
        kitchen_load_on = false;
      }if (a == "2"){
        digitalWrite(pin_2, LOW);
      }if (a == "b"){
        digitalWrite(pin_2, HIGH);
      }if (a == "3"){
        digitalWrite(pin_3, LOW);
      }if (a == "c"){
        digitalWrite(pin_3, HIGH);
      }if (a == "4"){
        digitalWrite(pin_4, LOW);
      }if (a == "d"){
        digitalWrite(pin_4, HIGH);
      }if (a == "5"){
        digitalWrite(pin_5, LOW);
      }if (a == "d"){
        digitalWrite(pin_5, HIGH);
      }if (a == "6"){
        digitalWrite(pin_6, LOW);
      }if (a == "e"){
        digitalWrite(pin_6, HIGH);
      }if (a == "8"){
        digitalWrite(pin_8, LOW);
      }if (a == "g"){
        digitalWrite(pin_8, HIGH);
      }
    }
  }
}
