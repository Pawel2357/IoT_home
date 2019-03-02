#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
 
// Connect to the WiFi
const char* ssid = "Dom";
const char* password = "izabelin";
const char* mqtt_server = "192.168.0.120";
String a;
 
WiFiClient espClient;
PubSubClient client(espClient);
 
const byte ledPin = 16;

void control_device(char receivedChar){
  Serial.write(receivedChar);
  if (receivedChar == '0')
    digitalWrite(ledPin, HIGH);
  if (receivedChar == '1')
    digitalWrite(ledPin, LOW);
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 //Serial.print("Message arrived [");
 //Serial.print(topic);
 //Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  control_device(receivedChar);
  }
}

 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   //Serial.print("Attempting MQTT connection...");
   // Attempt to connect
 if (client.connect("ESP8266 Client")) {
   //Serial.println("connected");
   // ... and subscribe to topic
  client.subscribe("ledStatus");
 } else {
   //Serial.print("failed, rc=");
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
}


void publish_data()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    String msg = Serial.readString();
    client.publish("test", (char*) msg.c_str());
  }
  //client.publish("test", "Hello from ESP32");
}


void loop()
{
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  publish_data();
}