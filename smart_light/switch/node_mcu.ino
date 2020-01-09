#include "DHT.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* lamp_controller_topic = "smart_lamp_controller"
char* MQTT_client =       lamp_controller_topic;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

uint8_t lamp_1 = D0;
uint8_t lamp_2 = D1;
uint8_t lamp_3 = D2;
uint8_t lamp_4 = D3;

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(MQTT_client)) {
   Serial.println("connected");
   // ... and subscribe to topic
   // client.subscribe("lamp_1");
 } else {
   Serial.print("failed, rc=");
   Serial.print(client.state());
   Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(2000);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(lamp_1, INPUT);
  pinMode(lamp_2, INPUT);
  pinMode(lamp_3, INPUT);
  pinMode(lamp_4, INPUT);
  // Wait for serial to initialize.
  while(!Serial) { }
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  button_1_State = digitalRead(lamp_1);
  button_2_State = digitalRead(lamp_2);
  button_3_State = digitalRead(lamp_3);
  button_4_State = digitalRead(lamp_4);
  if (button_1_State == HIGH){
    client.publish(lamp_controller_topic, (char*) ("1").c_str());
    delay(50);
  }elif(button_2_State == HIGH){
    client.publish(lamp_controller_topic, (char*) ("2").c_str());
    delay(50);
  }elif(button_3_State == HIGH){
    client.publish(lamp_controller_topic, (char*) ("3").c_str());
    delay(50);
  }elif(button_4_State == HIGH){
    client.publish(lamp_controller_topic, (char*) ("4").c_str());
    delay(50);
  }
}
