#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =        "pir_stairs";
char* lamp_control_topic = "lamp_stairs_control";
int break_time = 10000;

WiFiClient espClient;
PubSubClient client(espClient);


int inputPin1 = D1;               // choose the input pin (for PIR sensor)
int inputPin2 = D3;
int lampPin = D2;
bool pirState = LOW;             // we start, assuming no motion detected
bool pirState2 = LOW;             // we start, assuming no motion detected
int val1 = 0;                    // variable for reading the pin status
int val2 = 0;                    // variable for reading the pin status
unsigned long lastStatusLight1 = 0;
unsigned long lastStatusLight2 = 0;
unsigned long lastStatusWifi = 0;
unsigned long lastStatusMqtt = 0;
char* pir_1_topic =     "pir_stairs_1";
char* pir_2_topic =     "pir_stairs_2";

void setup_wifi2() {
    delay(10);
    // We start by connecting to a WiFi network
    //Serial.println();
    //Serial.print("Connecting to ");
    //Serial.println(ssid);

    if (millis() - lastStatusWifi > 500){
      Serial.println(".");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      lastStatusWifi = millis();
  
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
      }
    }
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
  if ((!client.connected()) && (millis() - lastStatusMqtt > 5000)) {
    Serial.print("Attempting MQTT connection...");
    yield();
    lastStatusMqtt = millis();
    // Attempt to connect
    if (client.connect(MQTT_client)) {
      Serial.println("connected");
      // ... and subscribe to topic
      client.subscribe(lamp_control_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void notify_pir_1(){
  val1 = digitalRead(inputPin1);  // read input value
  if (val1 == HIGH){            // check if the input is HIGH
    if (pirState == LOW) {
      Serial.print("motion_start_1");
      publish_data(pir_1_topic, String("motion_start_1"));
      pirState = HIGH;
    }
  }else{
    if (pirState == HIGH) {
      Serial.print("motion_end_1");
      publish_data(pir_1_topic, String("motion_end_1"));
      pirState = LOW;
      }
    }
}
  

void notify_pir_2(){
  val2 = digitalRead(inputPin2);  // read input value
  if (val2 == HIGH){            // check if the input is HIGH
    if (pirState2 == LOW) {
      Serial.print("motion_start_2");
      publish_data(pir_2_topic, String("motion_start_2"));
      pirState2 = HIGH;
    }
  }else{
    if (pirState2 == HIGH) {
      Serial.print("motion_end_2");
      publish_data(pir_2_topic, String("motion_end_2"));
      pirState2 = LOW;
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
    if ((millis() - lastStatusLight2 > break_time) && (millis() - lastStatusLight1 > break_time)) {
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
    if ((millis() - lastStatusLight2 > break_time) && (millis() - lastStatusLight1 > break_time)) {
      digitalWrite(lampPin, HIGH); // turn lamp off
      }
    }
  }

void control_lamp_mqtt(String message){
  Serial.print(message);
  if(message=="1"){
    digitalWrite(lampPin, LOW); // turn lamp on
    lastStatusLight1 = millis();
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
  control_lamp_mqtt(message);
}
 
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);
  pinMode(inputPin1, INPUT);     // declare sensor as input
  pinMode(inputPin2, INPUT);     // declare sensor as input
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, HIGH);
  Serial.begin(9600);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
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
      control_lamp_1();
      control_lamp_2();
      client.loop();
    }
  }
  // Things to do when Wifi is not required
  //control_lamp_1();
  //control_lamp_2();
}
