#include "DHTesp.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#define DHTPIN 4     // what digital pin the DHT22 is conected to
// #define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
#define FANPIN D6

DHTesp dht;

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =       "bathroom_climate";
char* climate_topic =     "bathroom_climate";
char* topic_subscribe =   "bathroom_fan";
unsigned long lastStatus = 0;

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    //Serial.println();
    // Serial.print("Connecting to ");
    // Serial.println(ssid);

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

void control_fan(int fan_speed){
  Serial.println("fan speed before");
  Serial.println(String(fan_speed));
  Serial.println("fan speed");
  Serial.print(fan_speed * 113);
  analogWrite(FANPIN, fan_speed * 113);
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   //Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(MQTT_client)) {
   delay(40);
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

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 String message;
 for (int i=0;i<length;i++) {
  char receivedChar;
  receivedChar = (char)payload[i];
  message += receivedChar;
  }
  control_fan(message.toInt());
}


void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void setup(){
  Serial.begin(9600);
  analogWriteFreq(4000);
  client.setCallback(callback);
  dht.setup(DHTPIN, DHTesp::DHT22); // Connect DHT sensor to GPIO 17

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  analogWrite(FANPIN, 0);
  //analogWriteFreq(25);
}

void loop()
{ 
  if ((!client.connected()) && (WiFi.status() != WL_CONNECTED)) {
    setup_wifi();
  }else{
    if (!client.connected()){
      reconnect();
    }else{
      if (millis() - lastStatus > 1000) {
        lastStatus = millis();
        // dht.setup(4, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
        // Report every 2 seconds.
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.getHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.getTemperature();
        Serial.print(t);
        Serial.print(h);          
        publish_data(climate_topic, String(h) + "," + String(t));
      }
      client.loop();
    }
  }
  delay(100);
}
