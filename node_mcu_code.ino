#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
 
// Connect to the WiFi
const char* ssid = "Dom";
const char* password = "izabelin";
const char* mqtt_server = "192.168.0.120";
 
WiFiClient espClient;
PubSubClient client(espClient);

void send_to_arduino(char receivedChar){
  Serial.write(receivedChar);
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  send_to_arduino(receivedChar);
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
   client.subscribe("sensors_3d_printing_room");
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
