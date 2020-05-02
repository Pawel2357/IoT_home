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
#define NUMLAMPS       1

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

const char* topic = "lamp_2";
const char* log_topic = "home_log";

WiFiClient espClient;
PubSubClient client(espClient);

int n_r = 255;
int n_g = 255;
int n_b = 255;
int n_i = 255;

void set_color(int lamp_nr, int r, int g, int b) {
  // Fix note: r swipped with g. This is grb diode.
  pixels.setPixelColor(lamp_nr, pixels.Color(n_g, n_r, n_b)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
}

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
  client.publish(log_topic, "lamp_2,"+ String(n_i) + "," + String(n_r) + "," + String(n_g) + "," + String(n_b));
  pixels.setBrightness(n_i);
  for(int l_nb = 0; l_nb < NUMLAMPS; l_nb++){
    set_color(l_nb, n_r, n_g, n_b);
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
 while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
   // Attempt to connect
 if (client.connect(topic)) {
   Serial.println("connected");
   // ... and subscribe to topic
   client.subscribe(topic);
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
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(n_i);
  for(int l_nb = 0; l_nb < NUMLAMPS; l_nb++){
    set_color(l_nb, n_r, n_g, n_b);
  }
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.publish(log_topic, "lamp_2,"+ String(n_i) + "," + String(n_r) + "," + String(n_g) + "," + String(n_b));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
