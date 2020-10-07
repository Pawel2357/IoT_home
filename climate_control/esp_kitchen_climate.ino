#include "DHTesp.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define DHTPIN 4     // what digital pin the DHT22 is conected to
// #define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHTesp dht;

// Connect to the WiFi
const char* ssid =        "Dom_2_4";
const char* password =    "izabelin";
const char* mqtt_server = "192.168.1.198";
char* MQTT_client =       "kitchen_climate";
char* climate_topic =     "kitchen_climate";

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    //Serial.println();
    //Serial.print("Connecting to ");
    //Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(MQTT_client)) {
   delay(40);
   // ... and subscribe to topic
   // client.subscribe("lamp_1");
 } else {
   //Serial.print("failed, rc=");
   //Serial.print(client.state());
   //Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

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

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(2000);
  dht.setup(DHTPIN, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
  Serial.print("0");

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Wait for serial to initialize.
  while(!Serial) { }
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // dht.setup(4, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
  // Report every 2 seconds.
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.getHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.getTemperature();
  //Serial.print(t);
  //Serial.print(h);
  delay(5000);           
  publish_data(climate_topic, String(h) + "," + String(t));
}
