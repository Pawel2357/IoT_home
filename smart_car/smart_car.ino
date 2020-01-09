#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Set up gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
const int MPU=0x68; 
int16_t AcX ,AcY, AcZ, Tmp, GyX, GyY, GyZ;
static const int RXPin = 13, TXPin = 12;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void send_to_computer(String data){
  Serial.write(data);
}

 
void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void read_6_dof(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  
  AcX=Wire.read()<<8|Wire.read();    
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read(); 
  data = String(AcX) + "," + String(AcY) + "," + String(AcZ) + "," + String(GyX) + "," + String(GyY) + "," + String(GyZ);
  send_to_computer(data);
}

void read_gps(){
  if (ss.available() > 0){
    yield();
    gps.encode(ss.read());
    yield();
    if (gps.location.isUpdated()){
      Serial.print("gps ok");
      String data;
      data = String(gps.location.lat(), 8) + "," + String(gps.location.lng(), 8) + "," + String(gps.speed.kmph());
      send_to_computer(data);
      delay(200);
    }
  }
}


void loop(){
  read_6_dof();
  read_gps();
}
