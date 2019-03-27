#include <DHT.h>
#define DHTTYPE DHT22 


String a; //Initialized variable to store recieved data
const int relay_pin = 13;
const int temp_hum_pin = 12;

DHT dht(temp_hum_pin, DHTTYPE);

int measure_air_quality(){
  int sensorValue = analogRead(A0);
  sensorValue = analogRead(A0);
  sensorValue = analogRead(A0);
  String strValue = String(sensorValue);
  return sensorValue;
}

float measure_temp(){
  float t = dht.readTemperature();
  t = dht.readTemperature();
  t = dht.readTemperature();
  return t;
}

float measure_humidity(){
  float h = dht.readHumidity();
  h = dht.readHumidity();
  h = dht.readHumidity();
  return h;
}

void setup() {
  //Serial Begin at 9600 Baud 
  pinMode(relay_pin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(relay_pin, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    // read the incoming byte:
    a = Serial.readString();

    // TODO: iterate over elements in this string or look at nodemcu

    // say what you got:
    if (a == "0"){
      digitalWrite(relay_pin, LOW); 
    }if (a == "1"){
      digitalWrite(relay_pin, HIGH);
    }if (a == "a"){
      Serial.print(measure_air_quality());
    }if (a == "t"){
      Serial.print(measure_temp());
    }if (a == "h"){
      Serial.print(measure_humidity());
    }
  }
}
