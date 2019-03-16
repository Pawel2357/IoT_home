#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Set up gps
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
static const int RXPin = 13, TXPin = 12;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

 
// Connect to the WiFi
const char* ssid = "Dom";
const char* password = "izabelin";
const char* mqtt_server = "40.121.36.126";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
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
   client.subscribe("smart_leaf_gps");
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
  ss.begin(GPSBaud);
}


void publish_data(String data)
{
  //char *cdata = data.c_str();
  Serial.print(data);
  char* cdata = strdup(data.c_str());
  client.publish("smart_leaf_gps", (char*) cdata);
}


void loop()
{
  Serial.print("dfdf");
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  delay(2000);
  publish_data("aaaa_test");

  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      String data;
      data = "lat," + String(gps.location.lat()) + ",lon," + gps.location.lng() + "speed" + gps.speed.kmph();
      publish_data(data);
    }
  }
}
