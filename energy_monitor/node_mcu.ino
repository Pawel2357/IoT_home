#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "0.0.0.0";

const char* mqtt_client_name = "energy_monitor";

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}


void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
   //Serial.print("Attempting MQTT connection...");
   yield();
   // Attempt to connect
 if (client.connect(mqtt_client_name)) {
   Serial.println("connected");
   // ... and subscribe to topic
 } else {
   //Serial.print("failed, rc=");
   //Serial.print(client.state());
   //Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

void publish_data()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    String msg = Serial.readString(); // TODO: This should be a separate method for recieving data from arduino
    client.publish("energy_monitor_0", (char*) msg.c_str());
  }
  //client.publish("test", "Hello from ESP32");
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  publish_data();
}
