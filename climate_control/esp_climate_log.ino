/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#include "DHT.h"

//Include the library
#include <MQUnifiedsensor.h>



#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


// Connect to the WiFi
const char* ssid =        "Dom_2_4";
const char* password =    "izabelin";
const char* mqtt_server = "192.168.1.198";
char* MQTT_client =       "climate_log";
char* humidity_t =        "humidity_climate";
char* temp_t =            "temp_climate";
char* co_t =              "co_climate";
char* alcohol_t =         "alcohol_climate";
char* co2_t =             "co2_climate";
char* tolueno_t =         "tolueno_climate";
char* nh4_t =             "nh4_climate";
char* acetona_t =         "acetona_climate";



#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

//Definitions
#define pin A0 //Analog input 0 of your arduino
#define type 135 //MQ135

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

//Declare Sensor
MQUnifiedsensor MQ135(pin, type);

//Variables
float CO, Alcohol, CO2, Tolueno, NH4, Acetona;


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


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
  MQ135.inicializar();

}


int timeSinceLastRead = 0;
void loop() {

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
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }


    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print("Humidity: ");
    Serial.print(h);
    publish_data(humidity_t, String(h));
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    publish_data(temp_t, String(t));
    Serial.print(" *C ");
    


    MQ135.update();

    CO =  MQ135.readSensor("CO"); // Return CO concentration
    Alcohol =  MQ135.readSensor("Alcohol"); // Return Alcohol concentration
    CO2 =  MQ135.readSensor("CO2"); // Return CO2 concentration
    Tolueno =  MQ135.readSensor("Tolueno"); // Return Tolueno concentration
    NH4 =  MQ135.readSensor("NH4"); // Return NH4 concentration
    Acetona =  MQ135.readSensor("Acetona"); // Return Acetona concentration

    Serial.println("***************************");
    Serial.println("Lectures for MQ-135");
    Serial.print("Volt: ");Serial.print(MQ135.getVoltage(false));Serial.println(" V"); 
    Serial.print("R0: ");Serial.print(MQ135.getR0());Serial.println(" Ohm"); 
    Serial.print("CO: ");Serial.print(CO,2);Serial.println(" ppm");
    publish_data(co_t, String(CO));
    Serial.print("Alcohol: ");Serial.print(Alcohol,2);Serial.println(" ppm");
    publish_data(alcohol_t, String(Alcohol));
    Serial.print("CO2: ");Serial.print(CO2,2);Serial.println(" ppm");
    publish_data(co2_t, String(CO2));
    Serial.print("Tolueno: ");Serial.print(Tolueno,2);Serial.println(" ppm");
    publish_data(tolueno_t, String(Tolueno));
    Serial.print("NH4: ");Serial.print(NH4,2);Serial.println(" ppm");
    publish_data(nh4_t, String(NH4));
    Serial.print("Acetona: ");Serial.print(Acetona,2);Serial.println(" ppm");
    publish_data(acetona_t, String(Acetona));
    Serial.println("***************************"); 
    

    timeSinceLastRead = 0;
  }
  delay(100);
  timeSinceLastRead += 100; 
  
}
