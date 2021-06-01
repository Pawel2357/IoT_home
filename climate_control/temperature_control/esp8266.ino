#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;  

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

const char* MQTT_client = "circulation_pump";
const char* topic_subscribe = "circulation_pump";

char* topic_publish_water_temp = "domestic_water_temp";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waitCount = 0;                 // counter
uint8_t conn_stat = 0;                       // Connection status for WiFi and MQTT:

unsigned long lastMillis = 0;
int interval = 5000;

uint8_t pin_0 = D0;

void send_to_arduino(char receivedChar){
  Serial.print(receivedChar);
  if(receivedChar == '1'){
    digitalWrite(pin_0, LOW);
  }
  if(receivedChar == '0'){
    digitalWrite(pin_0, HIGH);
  }
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
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    //WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
}

void reconnect() {
 // Loop until we're reconnected
   // Attempt to connect
 if (client.connect(MQTT_client)) {
   Serial.println("connected");
   // ... and subscribe to topic
   // client.subscribe("lamp_1");
 }
}

void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(pin_0, OUTPUT);
  digitalWrite(pin_0, HIGH);
  sensors.begin();
}

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void loop()
{
  // start of non-blocking connection setup section
  if ((WiFi.status() != WL_CONNECTED) && (conn_stat != 1)) { conn_stat = 0; }
  if ((WiFi.status() == WL_CONNECTED) && !client.connected() && (conn_stat != 3))  { conn_stat = 2; }
  if ((WiFi.status() == WL_CONNECTED) && client.connected() && (conn_stat != 5)) { conn_stat = 4;}
  switch (conn_stat) {
    case 0:                                                       // MQTT and WiFi down: start WiFi
      Serial.println("MQTT and WiFi down: start WiFi");
      setup_wifi();
      conn_stat = 1;
      break;
    case 1:                                                       // WiFi starting, do nothing here
      Serial.println("WiFi starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 2:                                                       // WiFi up, MQTT down: start MQTT
      Serial.println("WiFi up, MQTT down: start MQTT");
      reconnect();
      conn_stat = 3;
      waitCount = 0;
      break;
    case 3:                                                       // WiFi up, MQTT starting, do nothing here
      Serial.println("WiFi up, MQTT starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 4:                                                       // WiFi up, MQTT up: finish MQTT configuration
      Serial.println("WiFi up, MQTT up: finish MQTT configuration");
      client.subscribe(topic_subscribe);
      //mqttClient.publish(input_topic, Version);
      conn_stat = 5;                    
      break;
  }
  // end of non-blocking connection setup section

  // start section with tasks where WiFi/MQTT is required
  if (conn_stat == 5) {
    if(millis() - lastMillis > interval){
      sensors.requestTemperatures(); 
      float temperatureC = sensors.getTempCByIndex(0);
      Serial.println("temperatureC : "+ String(temperatureC));
      publish_data(topic_publish_water_temp, String(temperatureC));
      lastMillis = millis();
    }


    
  }
  client.loop(); 
}
