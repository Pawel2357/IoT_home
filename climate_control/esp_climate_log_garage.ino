/* standalone device with MQTT function
 *  
 * The requirements are as follows:
 * - it should run its main tasks regardless of the availability of WiFi/MQTT
 * - it should recover lost WiFi/MQTT without interrupting the main tasks
 * - unless other examples on the net it does not start the WiFi/MQTT connection in the setup() section
 *   since the device should start with its main functions immediately and do the connection setup later
 * 
 * Other features
 * - OTA over WiFi using the Arduino IDE
 * - MQTT over TLS for encrypted data transfer
 * - configuration parameters for device name, SSID, WAP-PW, MQTT broker username and password are stored in Preferences
 * 
 *  Feedback to improve this code is welcome
 *  Urs Eppenberger
 */

#include "DHTesp.h"

#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

DHTesp dht;

// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
char* MQTT_client =       "climate_log_garage";
char* climate_topic =     "data_climate_garage";

WiFiClient espClient;
PubSubClient client(espClient);

const int pin_mq_135 = 34; //Analog input 0 of your arduino
const int pin_dht = 36;

int mq_135_value;
unsigned long waitCount = 0;                 // counter
uint8_t conn_stat = 0;                       // Connection status for WiFi and MQTT:
                                             //
                                             // status |   WiFi   |    MQTT
                                             // -------+----------+------------
                                             //      0 |   down   |    down
                                             //      1 | starting |    down
                                             //      2 |    up    |    down
                                             //      3 |    up    |  starting
                                             //      4 |    up    | finalising
                                             //      5 |    up    |     up

unsigned long lastStatus = 0;                // counter in example code for conn_stat == 5
unsigned long lastTask = 0;                  // counter in example code for conn_stat <> 5

const char* Version = "{\"Version\":\"low_prio_wifi_v2\"}";
const char* Status = "{\"Message\":\"up and running\"}";

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

void publish_data(char* topic, String measure)
{
  client.publish(topic, (char*) measure.c_str());
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);                                            // config WiFi as client
  dht.setup(pin_dht, DHTesp::DHT22); // Connect DHT sensor to GPIO 17
  //setup_wifi();
  client.setServer(mqtt_server, 1883);
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
      //mqttClient.subscribe(output_topic);
      //mqttClient.publish(input_topic, Version);
      conn_stat = 5;                    
      break;
  }
// end of non-blocking connection setup section

// start section with tasks where WiFi/MQTT is required
  if (conn_stat == 5) {
    if (millis() - lastStatus > 1000) {                            // Start send status every 10 sec (just as an example)
      Serial.println(Status);

      mq_135_value = analogRead(pin_mq_135);       // read analog input pin 0
      Serial.print("AirQua=");
      Serial.print(mq_135_value); //, DEC);               // prints the value read
      Serial.println(" PPM"); 
      
      publish_data(climate_topic, String(0) + "," + String(0) + "," + String(mq_135_value));                      //      send status to broker
      client.loop();                                            //      give control to MQTT to send message to broker
      lastStatus = millis();                                        //      remember time of last sent status message
    }                                           // internal household function for OTA
    client.loop();                                              // internal household function for MQTT
  } 
// end of section for tasks where WiFi/MQTT are required

// start section for tasks which should run regardless of WiFi/MQTT
  if (millis() - lastTask > 1000) {                                 // Print message every second (just as an example)
    Serial.println("print this every second");
    lastTask = millis();
  }
  delay(100);
// end of section for tasks which should run regardless of WiFi/MQTT
}
