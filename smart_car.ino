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

void send_to_arduino(char receivedChar){
  Serial.write(receivedChar);
}

 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
 if (client.connect("ESP8266 Client")) {
   Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe("sensors_3d_printing_room");
 } else {
   Serial.print("failed, rc=");
   Serial.print(client.state());
   Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}
 
void setup()
{
  Serial.begin(9600);
  client.setServer(mqtt_server, 1883);
}


void publish_data(String data)
{
    // read the incoming byte:
    Serial.print(data);
    char* cdata = strdup(data.c_str());
    client.publish("smart_leaf_gps", (char*) cdata);
}


void loop()
{
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
  while (ss.available() > 0){
    gps.encode(ss.read());
    if (gps.location.isUpdated()){
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
      String data;
      data = "lat," + String(gps.location.lat()) + ",lon," + String(gps.location.lng()) + "speed" + String(gps.speed.kmph());
      publish_data(data);
    }
  }
}
