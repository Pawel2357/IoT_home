#include "Air_Quality_Sensor.h"

AirQualitySensor sensor(A0);

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid = "Dom_2_4";
const char* password = "izabelin";
const char* mqtt_server = "192.168.1.198";

const char* mqtt_client_name = "ventilation_bedroom";
const char* topic_subscribe = "ventilation_bedroom";
const char* topic_publish = "ventilation_bedroom_aq_log";

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t pin_0 = D1;
uint8_t pin_1 = D2;
uint8_t pin_2 = D3;

void send_to_arduino(char receivedChar){
  if(receivedChar == '0'){
    digitalWrite(pin_0, HIGH);
  }if(receivedChar == '1'){
    digitalWrite(pin_1, HIGH);
  }if(receivedChar == '2'){
    digitalWrite(pin_2, HIGH);
  }if(receivedChar == '4'){
    digitalWrite(pin_0, LOW);
  }if(receivedChar == '5'){
    digitalWrite(pin_1, LOW);
  }if(receivedChar == '6'){
    digitalWrite(pin_2, LOW);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  send_to_arduino(receivedChar);
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

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(pin_0, OUTPUT);
  digitalWrite(pin_0, HIGH);
  pinMode(pin_1, OUTPUT);
  digitalWrite(pin_1, HIGH);
  pinMode(pin_2, OUTPUT);
  digitalWrite(pin_2, HIGH);

  delay(20000);

  if (sensor.init()) {
      Serial.println("Sensor ready.");
  } else {
      Serial.println("Sensor ERROR!");
  }
}


void loop()
{
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  delay(1000);
  int quality = sensor.slope();
  Serial.println(String(quality));
  client.publish(topic_publish, (String(quality) + "," + String(sensor.getValue())).c_str(), true);
}
