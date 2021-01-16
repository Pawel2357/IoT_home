#include "DHTesp.h"

#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =       "pir_stairs";
char* climate_topic =     "pir_stairs";

WiFiClient espClient;
PubSubClient client(espClient);

 
int inputPin = D1;               // choose the input pin (for PIR sensor)
int inputPin2 = D3;
int lampPin = D2;
int pirState = LOW;             // we start, assuming no motion detected
int pirState2 = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
unsigned long lastStatusLight1 = 0;
unsigned long lastStatusLight2 = 0;
char* pir_1_topic =     "pir_stairs_1";
char* pir_2_topic =     "pir_stairs_2";

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

void notify_pir_1(){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH){            // check if the input is HIGH
    if (pirState == LOW) {
      publish_data(pir_1_topic, String("motion_start"));
      pirState = HIGH;
    }
  else{
    if (pirState == HIGH) {
      publish_data(pir_1_topic, String("motion_end"));
      pirState == LOW;
      }
    }
  }
}

void notify_pir_2(){
  val2 = digitalRead(inputPin2);  // read input value
  if (val2 == HIGH){            // check if the input is HIGH
    if (pirState2 == LOW) {
      publish_data(pir_2_topic, String("motion_start"));
      pirState2 = HIGH;
    }
  else{
    if (pirState2 == HIGH) {
      publish_data(pir_2_topic, String("motion_end"));
      pirState2 == LOW;
      }
    }
  }
}

void control_lamp_2(){
  val2 = digitalRead(inputPin2);  // read input value
  if (val2 == HIGH){            // check if the input is HIGH
    lastStatusLight2 = millis();
    digitalWrite(lampPin, LOW); // turn lamp on
    }
  else{
    if ((millis() - lastStatusLight2 > 45000) && (millis() - lastStatusLight1 > 45000)) {
      digitalWrite(lampPin, HIGH); // turn lamp off
      }
    }
  }

void control_lamp_1(){
  val1 = digitalRead(inputPin1);  // read input value
  if (val1 == HIGH){            // check if the input is HIGH
    lastStatusLight1 = millis();
    digitalWrite(lampPin, LOW); // turn lamp on
    }
  else{
    if ((millis() - lastStatusLight2 > 45000) && (millis() - lastStatusLight1 > 45000)) {
      digitalWrite(lampPin, HIGH); // turn lamp off
      }
    }
  }
 
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(inputPin2, INPUT);     // declare sensor as input
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, HIGH);
  Serial.begin(9600);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Wait for serial to initialize.
  while(!Serial) { }
}
 
void loop(){
  if ((!client.connected()) && (WiFi.status() != WL_CONNECTED)) {
    setup_wifi();
  }else{
    if (!client.connected()){
      reconnect();
    }else{
      notify_pir_1();
      notify_pir_2();
      client.loop();
    }
  }
  // Things to do when Wifi is not required
  control_lamp_1();
  control_lamp_2();
}
