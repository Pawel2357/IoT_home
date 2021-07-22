#include "DHTesp.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =        "pir_stairs";
char* lamp_control_topic = "lamp_stairs_control";
char* climate_topic =     "data_climate_stairs";
int break_time = 50000;

DHTesp dht;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waitCount = 0;                 // counter
uint8_t conn_stat = 0;                       // Connection status for WiFi and MQTT:
int inputPin1 = D1;               // choose the input pin (for PIR sensor)
int inputPin2 = D3;
int lampPin = D2;
bool pirState = LOW;             // we start, assuming no motion detected
bool pirState2 = LOW;             // we start, assuming no motion detected
int val1 = 0;                    // variable for reading the pin status
int val2 = 0;                    // variable for reading the pin status
unsigned long lastStatusLight1 = 0;
unsigned long lastStatusLight2 = 0;
unsigned long lastStatusWifi = 0;
unsigned long lastStatusMqtt = 0;
unsigned long lastStatusClimate = 0;
char* pir_1_topic =     "pir_stairs_1";
char* pir_2_topic =     "pir_stairs_2";
const int pin_dht = D4;

const char* Status = "{\"Message\":\"up and running\"}";

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
}

void reconnect() {
 // Loop until we're reconnected
   // Attempt to connect
 if (client.connect(MQTT_client)) {
   Serial.println("connected");
   // ... and subscribe to topic
   // client.subscribe("lamp_1");
 }
}

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void notify_climate(){
  if (millis() - lastStatusClimate > 2000) {
    float h = dht.getHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.getTemperature();
    Serial.print(t);
    Serial.print(h);
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }else{
      publish_data(climate_topic, String(h) + "," + String(t) + "," + String(0));
    }
    lastStatusClimate = millis();
  }
}

void notify_pir_1(){
  val1 = digitalRead(inputPin1);  // read input value
  if (val1 == HIGH){            // check if the input is HIGH
    if (pirState == LOW) {
      Serial.print("motion_start_1");
      publish_data(pir_1_topic, String("motion_start_1"));
      pirState = HIGH;
    }
  }else{
    if (pirState == HIGH) {
      Serial.print("motion_end_1");
      publish_data(pir_1_topic, String("motion_end_1"));
      pirState = LOW;
      }
    }
}
  

void notify_pir_2(){
  val2 = digitalRead(inputPin2);  // read input value
  if (val2 == HIGH){            // check if the input is HIGH
    if (pirState2 == LOW) {
      Serial.print("motion_start_2");
      publish_data(pir_2_topic, String("motion_start_2"));
      pirState2 = HIGH;
    }
  }else{
    if (pirState2 == HIGH) {
      Serial.print("motion_end_2");
      publish_data(pir_2_topic, String("motion_end_2"));
      pirState2 = LOW;
      }
    }
}

void control_lamp_2(){
  val2 = digitalRead(inputPin2);  // read input value
  if (val2 == HIGH){            // check if the input is HIGH
    lastStatusLight2 = millis();
    digitalWrite(lampPin, LOW); // turn lamp on
    }
  else{
    if ((millis() - lastStatusLight2 > break_time) && (millis() - lastStatusLight1 > break_time)) {
      digitalWrite(lampPin, HIGH); // turn lamp off
      }
    }
  }

void control_lamp_1(){
  val1 = digitalRead(inputPin1);  // read input value
  if (val1 == HIGH){            // check if the input is HIGH
    lastStatusLight1 = millis();
    digitalWrite(lampPin, LOW); // turn lamp on
    }
  else{
    if ((millis() - lastStatusLight2 > break_time) && (millis() - lastStatusLight1 > break_time)) {
      digitalWrite(lampPin, HIGH); // turn lamp off
      }
    }
  }

void control_lamp_mqtt(String message){
  Serial.print(message);
  if(message=="1"){
    digitalWrite(lampPin, LOW); // turn lamp on
    lastStatusLight1 = millis();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // get subscribed message char by char
  // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
  String message="";
  for (int i=0;i<length;i++) {
    char receivedChar;
    receivedChar = (char)payload[i];
    message += receivedChar;
  }
  Serial.print(message);
  control_lamp_mqtt(message);
}
 
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  pinMode(inputPin1, INPUT);     // declare sensor as input
  pinMode(inputPin2, INPUT);     // declare sensor as input
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, HIGH);
  dht.setup(pin_dht, DHTesp::DHT22);
  client.setCallback(callback);
  client.setServer(mqtt_server, 1883);
}
 
void loop() {                                                     // with current code runs roughly 400 times per second
// start of non-blocking connection setup section
  if ((WiFi.status() != WL_CONNECTED) && (conn_stat != 1)) { conn_stat = 0; }
  if ((WiFi.status() == WL_CONNECTED) && !client.connected() && (conn_stat != 3))  { conn_stat = 2; }
  if ((WiFi.status() == WL_CONNECTED) && client.connected() && (conn_stat != 5)) { conn_stat = 4;}
  switch (conn_stat) {
    case 0:                                                       // MQTT and WiFi down: start WiFi
      Serial.println("MQTT and WiFi down: start WiFi");
      setup_wifi();
      conn_stat = 1;
      break;
    case 1:                                                       // WiFi starting, do nothing here
      Serial.println("WiFi starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 2:                                                       // WiFi up, MQTT down: start MQTT
      Serial.println("WiFi up, MQTT down: start MQTT");
      reconnect();
      conn_stat = 3;
      waitCount = 0;
      break;
    case 3:                                                       // WiFi up, MQTT starting, do nothing here
      Serial.println("WiFi up, MQTT starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 4:                                                       // WiFi up, MQTT up: finish MQTT configuration
      Serial.println("WiFi up, MQTT up: finish MQTT configuration");
      client.subscribe(lamp_control_topic);
      //mqttClient.publish(input_topic, Version);
      conn_stat = 5;                    
      break;
  }
// end of non-blocking connection setup section

// start section with tasks where WiFi/MQTT is required
  if (conn_stat == 5) {
    notify_pir_1();
    notify_pir_2();                                     
    notify_climate();
    client.loop();                                              // internal household function for MQTT
  } 
// end of section for tasks where WiFi/MQTT are required

// start section for tasks which should run regardless of WiFi/MQTT
  if (waitCount > 550){
    ESP.restart();
  }
  control_lamp_1();
  control_lamp_2();
  delay(100);
// end of section for tasks which should run regardless of WiFi/MQTT
}
