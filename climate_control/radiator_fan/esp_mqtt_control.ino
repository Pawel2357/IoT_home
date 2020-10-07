#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid = "Dom_2_4";
const char* password = "izabelin";
const char* mqtt_server = "192.168.1.198";

const char* mqtt_client_name = "radiator_living_room";
const char* topic_subscribe = "radiator_living_room";

WiFiClient espClient;
PubSubClient client(espClient);

void send_to_arduino(char receivedChar){
  Serial.print(receivedChar);
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
        //Serial.print(".");
    }
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   //Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(mqtt_client_name)) {
   //Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe(topic_subscribe);
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
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
  reconnect();
  }
  client.loop();
}
