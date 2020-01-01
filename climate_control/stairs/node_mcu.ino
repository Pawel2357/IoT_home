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

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

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

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(2000);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Wait for serial to initialize.
  while(!Serial) { }
}

int timeSinceLastRead = 0;

void loop(){

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    // Report every 2 seconds.
  if(timeSinceLastRead > 20000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    delay(3000); 
    Serial.println(String(h) + ' ' + String(t));          

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }
                        // wait 100ms for next reading

    publish_data(climate_topic, String(h) + "," + String(t));
    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100; 
}
