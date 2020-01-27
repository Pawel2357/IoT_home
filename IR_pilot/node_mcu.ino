#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define CH0 0xFF6897
#define CH1 0xFF30CF
#define CH2 0xFF18E7
#define PLUS 0xFFA857
#define MINUS 0xFFE01F
#define EQ 0xFF906F

// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

const char* topic_1 = "lamp_1";
const char* topic_2 = "lamp_2";
const char* topic_3 = "lamp_3";
const char* topic_ventilation = "ventilation_living_room";

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t kRecvPin = 5;

IRrecv irrecv(kRecvPin);

decode_results results;

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
   // Attempt to connect
 if (client.connect("IR_lights")) {
   Serial.println("connected");
   // ... and subscribe to topic
 } else {
   Serial.print("failed, rc=");
   Serial.print(client.state());
   Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

void set_color(int i, int r, int g, int b, const char* topic) {
  // Fix note: r swipped with g. This is grb diode.
  client.publish(topic, ("i" + String(i)).c_str());
  client.publish(topic, ("r" + String(r)).c_str());
  client.publish(topic, ("g" + String(g)).c_str());
  client.publish(topic, ("b" + String(b)).c_str());
}

void off_ventilation(){
  client.publish(topic_ventilation, "4");
  client.publish(topic_ventilation, "5");
}

void slow_ventilation(){
  client.publish(topic_ventilation, "0");
  client.publish(topic_ventilation, "5");
}

void fast_ventilation(){
  client.publish(topic_ventilation, "0");
  client.publish(topic_ventilation, "1");
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  irrecv.enableIRIn();  // Start the receiver
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    irrecv.resume();  // Receive the next value
    //int res = String(results.value, DEC);
    if(results.value == CH1){
      Serial.println("got 1");
      set_color(255, 255, 255, 255, topic_1);
      set_color(255, 255, 255, 255, topic_2);
      set_color(255, 255, 255, 255, topic_3);
    }
    if(results.value == CH0){
      Serial.println("got 1");
      set_color(0, 255, 255, 255, topic_1);
      set_color(0, 255, 255, 255, topic_2);
      set_color(0, 255, 255, 255, topic_3);
    }
    if(results.value == CH2){
      Serial.println("got 2");
      set_color(4, 255, 255, 0, topic_1);
      set_color(4, 255, 255, 0, topic_2);
      set_color(4, 255, 255, 0, topic_3);
    }
    if(results.value == PLUS){
      Serial.println("fast vent");
      fast_ventilation();
    }
    if(results.value == MINUS){
      Serial.println("slow vent");
      slow_ventilation();
    }
    if(results.value == EQ){
      Serial.println("off vent");
      off_ventilation();
    }
  }
  delay(100);
}
