#include "DHTesp.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// #define DHTPIN 4     // what digital pin the DHT22 is conected to
// #define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHTesp dht;

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =       "climate_log_garage";
char* climate_topic =     "data_climate_garage";

WiFiClient espClient;
PubSubClient client(espClient);

const int pin_mq_135 = 0; //Analog input 0 of your arduino
const int pin_mq_2 = 1;

int mq_135_value;

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
  dht.setup(4, DHTesp::DHT22); // Connect DHT sensor to GPIO 17

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
  Serial.print(t);
  Serial.print(h);
  mq_135_value = analogRead(pin_mq_135);       // read analog input pin 0
  Serial.print("AirQua=");
  Serial.print(mq_135_value); //, DEC);               // prints the value read
  Serial.println(" PPM");
  delay(7000);           

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(mq_135_value)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  
                        // wait 100ms for next reading

    publish_data(climate_topic, String(h) + "," + String(t) + "," + String(mq_135_value));

  delay(100);
}
