#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


int sensor_0 = 13;  // Digital pin D7
int sensor_1 = 12; // Digital pin D6
uint8_t pin_lamp = D1;


// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

const char* mqtt_client_name = "lamp_stairs";
const char* topic_subscribe = "lamp_stairs";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // get subscribed message char by char
  // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  String val = (char *)payload;
  int number = val.toInt();
  // here code for action based on number value
}

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
 if (client.connect(mqtt_client_name)) {
   Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe(topic_subscribe);
 } else {
   Serial.print("failed, rc=");
   Serial.print(client.state());
   Serial.println(" try again in 5 seconds");
   // Wait 5 seconds before retrying
   delay(5000);
   }
 }
}

void setup() {

  pinMode(sensor_0, INPUT);   // declare sensor as input
  pinMode(sensor_1, INPUT);   // declare sensor as input
  pinMode(pin_lamp, OUTPUT);
  digitalWrite(pin_lamp, LOW);  // turn on light for 35 sec
  // Wait 35 seconds
  delay(35000);
  digitalWrite(pin_lamp, HIGH);  // turn on light for 5 sec
  
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
    long state_0 = digitalRead(sensor_0);
    long state_1 = digitalRead(sensor_1);
    
    if(state_0 == HIGH || state_1 == HIGH) {
      Serial.println("Motion detected!");
      digitalWrite(pin_lamp, LOW);  // turn on light for 35 sec
      delay(35000);
    }
    else {
      digitalWrite(pin_lamp, HIGH);  // turn off light for 5 sec
      Serial.println("Motion absent!");
      delay(500);
    }
    // Fix note: r swipped with g. This is grb diode.


  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
