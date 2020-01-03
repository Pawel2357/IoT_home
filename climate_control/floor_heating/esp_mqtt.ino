#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid = "Dom_2_4";
const char* password = "izabelin";
const char* mqtt_server = "xyz";

const char* mqtt_client_name = "kitchen_floor_heating";
const char* topic_subscribe = "kitchen_floor_heating";
const char* topic_kitchen_floor_temp = "kitchen_floor_temp";

#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000    

uint8_t pin_0 = D0;
uint8_t pin_1 = D1;
uint8_t pin_2 = D2;
uint8_t pin_3 = D3;

WiFiClient espClient;
PubSubClient client(espClient);

float measure_voltage() {
  uint8_t i;
  float average;
  int samples[NUMSAMPLES];
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
  
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
 
  Serial.print("Average analog reading "); 
  Serial.println(average);
  
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  
  Serial.print("Temperature "); 
  Serial.print(steinhart);
  Serial.println(" *C");
  
  delay(1000);
  return steinhart;
}

void send_to_arduino(char receivedChar){
  if(receivedChar == '0'){
    digitalWrite(pin_0, LOW);
  }if(receivedChar == '1'){
    digitalWrite(pin_1, LOW);
  }if(receivedChar == '2'){
    digitalWrite(pin_2, LOW);
  }if(receivedChar == '3'){
    digitalWrite(pin_3, LOW);
  }if(receivedChar == '4'){
    digitalWrite(pin_0, HIGH);
  }if(receivedChar == '5'){
    digitalWrite(pin_1, HIGH);
  }if(receivedChar == '6'){
    digitalWrite(pin_2, HIGH);
  }if(receivedChar == '7'){
    digitalWrite(pin_3, HIGH);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  if(receivedChar == 't'){
    client.publish(topic_kitchen_floor_temp, String(measure_voltage());
  }else{
    send_to_arduino(receivedChar);
  }
  }
}


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}


void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   //Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(mqtt_client_name)) {
   Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe(topic_subscribe);
 } else {
   Serial.print("failed, rc=");
   //Serial.print(client.state());
   //Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

void turn_off_all(){
  digitalWrite(pin_0, LOW);
  digitalWrite(pin_1, LOW);
  digitalWrite(pin_2, LOW);
  digitalWrite(pin_3, LOW);
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(pin_0, OUTPUT);
  pinMode(pin_1, OUTPUT);
  pinMode(pin_2, OUTPUT);
  pinMode(pin_3, OUTPUT);
  turn_off_all()
}


void loop()
{
  if (!client.connected()) {
    turn_off_all()
    reconnect();
  }
  client.loop();
}
