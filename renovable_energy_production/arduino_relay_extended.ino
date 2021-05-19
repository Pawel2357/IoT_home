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
unsigned long lastStatus2 = 0;
unsigned long lastStatus3 = 0;
unsigned long lastStatus4 = 0;
unsigned long lastStatus_check = 0;

boolean inverter_running = false;
boolean kitchen_load_on = false;
boolean car_charging_on = false;
boolean comp_load_on = false;
boolean ind_kitchen_load_on = false;
boolean heat_pump_load_on = false;

double power_kitchen;
double power_inverter;
double power_ind_kitchen;
double power_heat_pump;
double power_comp;
double power;

int max_inverter_power = 2560;

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
  if (millis() - lastStatus_check > 80){
    power_inverter = emon4.calcIrms(1480) * 230.0;  // Calculate Irms only
    //power_kitchen = emon1.calcIrms(1480) * 230.0;
    //power_ind_kitchen = emon3.calcIrms(1480) * 230.0;
    //power_heat_pump = emon2.calcIrms(1480) * 230.0;
    
    power_kitchen = 200;
    power_ind_kitchen = 200;
    power_heat_pump = 200;
    if(power_inverter > max_inverter_power){
      // heat pump priority
      if(heat_pump_load_on==true && power_heat_pump > 300 && (comp_load_on==true || car_charging_on==true || kitchen_load_on==true || ind_kitchen_load_on==true)){
        digitalWrite(pin_6, HIGH);
        comp_load_on = false;
        digitalWrite(pin_2, HIGH); // turn off car charging
        car_charging_on = false;
        digitalWrite(pin_1, HIGH);
        kitchen_load_on = false;

        power_ind_kitchen = false;
      }else if(power_ind_kitchen > 1900 && power_ind_kitchen < max_inverter_power){  // dishwasher priority
        // turn off comp, kitchen, car charging
        digitalWrite(pin_6, HIGH);
        comp_load_on==false;
        digitalWrite(pin_2, HIGH); // turn off car charging
        car_charging_on = false;
        digitalWrite(pin_1, HIGH);
        kitchen_load_on = false;
      }else if(power_inverter < (max_inverter_power + 120) && car_charging_on==false && comp_load_on==true){
        digitalWrite(pin_6, HIGH);
        comp_load_on==false;
      }else if(power_inverter < (max_inverter_power + 1430) && car_charging_on==true && kitchen_load_on==true){
        digitalWrite(pin_2, HIGH);
        car_charging_on = false;
      }else{
        digitalWrite(pin_1, HIGH);
        digitalWrite(pin_2, HIGH);
        digitalWrite(pin_3, HIGH);
        digitalWrite(pin_4, HIGH);
        digitalWrite(pin_5, HIGH);
        digitalWrite(pin_6, HIGH);
        digitalWrite(pin_8, HIGH);
        car_charging_on = false;
        kitchen_load_on = false;
        comp_load_on = false;
        ind_kitchen_load_on = false;
      }
    }
    lastStatus_check=millis();
  }

  if (millis() - lastStatus > 3000){
    Serial.print(String(power_inverter) + ",Power_inverter," + String(power_ind_kitchen) + ",Power_kitchen," + String(power_heat_pump) + ",Power_hp," + String(power_kitchen) + ",Power_7");
  }
  

  if (Serial.available() > 0) {
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
      car_charging_on = false;
      kitchen_load_on = false;
      comp_load_on = false;
      ind_kitchen_load_on = false;
      heat_pump_load_on = false;
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
      car_charging_on = false;
      comp_load_on = false;
      ind_kitchen_load_on = false;
      heat_pump_load_on = false;
      delay(100);
      digitalWrite(pin_7, HIGH);
      delay(900);
      inverter_running = false;
    }
    if (inverter_running == true){
      // turn on power_7
      if (a == "1" && power_kitchen < 500){
        // check if big load
        digitalWrite(pin_1, LOW);
        kitchen_load_on = true;
      }if (a == "a"){
        digitalWrite(pin_1, HIGH);
        kitchen_load_on = false;
      }if (a == "2" && power_inverter < 950){
        digitalWrite(pin_2, LOW);
        car_charging_on = true;
      }if (a == "b"){
        digitalWrite(pin_2, HIGH);
        car_charging_on = false;
      }if (a == "3" && power_heat_pump < 300){
        digitalWrite(pin_3, LOW);
        heat_pump_load_on = true;
      }if (a == "c"){
        digitalWrite(pin_3, HIGH);
        heat_pump_load_on = false;
      }if (a == "4" && power_ind_kitchen < 400){
        digitalWrite(pin_4, LOW);
        ind_kitchen_load_on = true;
      }if (a == "d"){
        digitalWrite(pin_4, HIGH);
        ind_kitchen_load_on = false;
      }if (a == "5"){
        digitalWrite(pin_5, LOW);
      }if (a == "d"){
        digitalWrite(pin_5, HIGH);
      }if (a == "6" && power_inverter < max_inverter_power - 400){
        digitalWrite(pin_6, LOW);
        comp_load_on = true;
      }if (a == "e"){
        digitalWrite(pin_6, HIGH);
        comp_load_on = false;
      }if (a == "8"){
        digitalWrite(pin_8, LOW);
      }if (a == "g"){
        digitalWrite(pin_8, HIGH);
      }
    }
  }
}
