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
#define NUMPIXELS      100
#define NUMLAMPS       100

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Connect to the WiFi
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";

const char* topic = "led_kitchen";
const char* log_topic = "home_log";
char* MQTT_client = "kitchen_led";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waitCount = 0;                 // counter
uint8_t conn_stat = 0;                       // Connection status for WiFi and MQTT:

int n_r = 255;
int n_g = 255;
int n_b = 255;
int n_i = 0;

const char* Status = "{\"Message\":\"up and running\"}";

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
  //client.publish(log_topic, "lamp_2,"+ String(n_i) + "," + String(n_r) + "," + String(n_g) + "," + String(n_b));
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

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(n_i);
  for(int l_nb = 0; l_nb < NUMLAMPS; l_nb++){
    set_color(l_nb, n_r, n_g, n_b);
  }
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //client.publish(log_topic, "lamp_2,"+ String(n_i) + "," + String(n_r) + "," + String(n_g) + "," + String(n_b));
}

void loop() {                                                     // with current code runs roughly 400 times per second
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
      client.subscribe(topic);
      //mqttClient.publish(input_topic, Version);
      conn_stat = 5;                    
      break;
  }
// end of non-blocking connection setup section

// start section with tasks where WiFi/MQTT is required
  if (conn_stat == 5) {
    client.loop();                                              // internal household function for MQTT
  } 
// end of section for tasks where WiFi/MQTT are required

// start section for tasks which should run regardless of WiFi/MQTT
  if (waitCount > 550){
    ESP.restart();
  }
// end of section for tasks which should run regardless of WiFi/MQTT
}
