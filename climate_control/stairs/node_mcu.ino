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
char* MQTT_client =       "climate_stairs";
char* climate_topic =     "climate_stairs";
char* climate_topic_log =     "climate_stairs_log";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

uint8_t pin_0 = D0;
uint8_t pin_1 = D1;
uint8_t pin_2 = D2;
uint8_t pin_3 = D3;

void send_to_arduino(char receivedChar){
  Serial.print(receivedChar);
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

void turn_off_all(){
  digitalWrite(pin_0, LOW);
  digitalWrite(pin_1, LOW);
  digitalWrite(pin_2, LOW);
  digitalWrite(pin_3, LOW);
}

void publish_data(char* topic, String measure){
  client.publish(topic, (char*) measure.c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  if(receivedChar == 't'){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }else{
      publish_data(climate_topic_log, String(h) + "," + String(t));
    }
  }else{
    send_to_arduino(receivedChar);
  }
 }
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(2000);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(pin_0, OUTPUT);
  pinMode(pin_1, OUTPUT);
  pinMode(pin_2, OUTPUT);
  pinMode(pin_3, OUTPUT);
  turn_off_all();

  // Wait for serial to initialize.
  while(!Serial) { }
}

void loop(){
  if (!client.connected()) {
    turn_off_all();
    reconnect();
  }
  client.loop();
}
