#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

WiFiClient espClient;
PubSubClient client(espClient);

int n_r = 255;
int n_g = 255;
int n_b = 255;
int n_i = 255;
int sensor = 13;  // Digital pin D7



/*
 * TrueRandom - A true random number generator for Arduino.
 * This is variant of original work originally implemented as:
 * https://code.google.com/archive/p/tinkerit/ https://github.com/Cathedrow/TrueRandom
 * Copyright (c) 2010 Peter Knight, Tinker.it! All rights reserved.
 * Now modified for the ESP8266
 */

#ifndef ESP8266TrueRandom_h
#define ESP8266TrueRandom_h

#include <Arduino.h>
#include <inttypes.h>

class ESP8266TrueRandomClass
{
  public:
  ICACHE_FLASH_ATTR ESP8266TrueRandomClass();
  ICACHE_FLASH_ATTR int rand();
  ICACHE_FLASH_ATTR long random();
  ICACHE_FLASH_ATTR long random(long howBig);
  ICACHE_FLASH_ATTR long random(long howsmall, long how);
  ICACHE_FLASH_ATTR int randomBit(void);
  ICACHE_FLASH_ATTR char randomByte(void);
  ICACHE_FLASH_ATTR void memfill(char* location, int size);
  ICACHE_FLASH_ATTR void mac(uint8_t* macLocation);
  ICACHE_FLASH_ATTR void uuid(uint8_t* uuidLocation);
  ICACHE_FLASH_ATTR String uuidToString(uint8_t* uuidLocation);
    bool useRNG;
  private:
    unsigned long lastYield;
    ICACHE_FLASH_ATTR int randomBitRaw(void);
    ICACHE_FLASH_ATTR int randomBitRaw2(void);
};
extern ESP8266TrueRandomClass ESP8266TrueRandom;
#endif



void callback(char* topic, byte* payload, unsigned int length) {
  // get subscribed message char by char
  // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
  payload[length] = '\0'; // Make payload a string by NULL terminating it.
  String wholeVal = (char *)payload;
  int number = wholeVal.substring(1, wholeVal.length()).toInt();
  String data_type = wholeVal.substring(0, 1);
  Serial.print(number);
  Serial.println(data_type);
  String r = String('r');
  String g = String('g');
  String b = String('b');
  String i = String('i');
  if(data_type == r){
    n_r = number;
  }
  if(data_type == g){
    n_g = number;
  }
  if(data_type == b){
    n_b = number;
  }
  if(data_type == i){
    n_i = number;
  }

  pixels.setBrightness(n_i);
  // Fix note: r swipped with g. This is grb diode.
  pixels.setPixelColor(0, pixels.Color(n_g, n_r, n_b)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
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
 if (client.connect("lamp_1")) {
   Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe("lamp_1");
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
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  pinMode(sensor, INPUT);   // declare sensor as input

  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {
    long state = digitalRead(sensor);
    if(state == HIGH) {
      Serial.println("Motion detected!");
      pixels.setBrightness(255);
      pixels.setPixelColor(0, pixels.Color(random(256), random(256), random(256))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(25000);
    }
    else {
      pixels.setBrightness(0);
      pixels.setPixelColor(0, pixels.Color(random(256), random(256), random(256))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
      Serial.println("Motion absent!");
      delay(1000);
    }
    // Fix note: r swipped with g. This is grb diode.


  //if (!client.connected()) {
  //  reconnect();
  //}
  //client.loop();
}
