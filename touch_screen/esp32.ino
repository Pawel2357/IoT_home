// Example of drawing a graphical "switch" and using
// the touch screen to change it's state.

// This sketch does not use the libraries button drawing
// and handling functions.

// Based on Adafruit_GFX library onoffbutton example.

// Touch handling for XPT2046 based screens is handled by
// the TFT_eSPI library.

// Calibration data is stored in SPIFFS so we need to include it
#include "FS.h"

#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "xyz";
const char* password = "xyz";
const char* mqtt_server = "xyz";
const char* mqtt_client_name = "touch_screen";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waitCount = 0;                 // counter
uint8_t conn_stat = 0;                       // Connection status for WiFi and MQTT:                    

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData5"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

boolean SwitchOn = false;
boolean SwitchOn2 = false;

// Comment out to stop drawing black spots
#define BLACK_SPOT

// Switch position and size
#define FRAME_X 5
#define FRAME_Y 5
#define FRAME_X2 5
#define FRAME_Y2 35
#define FRAME_W 140
#define FRAME_H 30

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

// Red zone size
#define REDBUTTON_X2 FRAME_X2
#define REDBUTTON_Y2 FRAME_Y2
#define REDBUTTON_W2 (FRAME_W/2)
#define REDBUTTON_H2 FRAME_H

// Green zone size
#define GREENBUTTON_X2 (REDBUTTON_X2 + REDBUTTON_W)
#define GREENBUTTON_Y2 FRAME_Y2
#define GREENBUTTON_W2 (FRAME_W/2)
#define GREENBUTTON_H2 FRAME_H




/*// Keypad start position, key sizes and spacing
#define KEY_X 200 // Center of key
#define KEY_Y 30
#define KEY_W 35 // Width and height
#define KEY_H 30 */
#define KEY_TEXTSIZE 1 // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1

// Create Signal Button


// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key_plus;
TFT_eSPI_Button key_minus;
TFT_eSPI_Button key_set_auto;
TFT_eSPI_Button key_climate_go_to_menu;
TFT_eSPI_Button key_climate;

float target_temp = 20.0;
String messageTemp = "-";
String messageHumid = "-";
String messageAir = "-";
String messageTemp_k = "-";
String messageTemp_b = "-";
String messageHumid_k = "-";
String messageHumid_b = "-";

String screen_mode = "climate"; // possible modes main_menu, cimate, solar, lights, other, (energy)

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  tft.init();
  tft.setRotation(1);  

  // call screen calibration
  touch_calibrate();

  // clear screen
  tft.fillScreen(TFT_GREEN);
  tft.setFreeFont(LABEL1_FONT);
  tft.setTextSize(1);
  tft.setTextFont(1);
  tft.setTextColor(TFT_RED, TFT_GREEN);
  // Set the rotation before we calibrate
  // Draw button (this example does not use library Button class)
/*  redBtn();
  redBtn2();

  tft.setFreeFont(LABEL1_FONT);
  key_plus.initButton(&tft, 200, 30, 35, 30, TFT_WHITE, TFT_RED, TFT_WHITE, "+", KEY_TEXTSIZE); 
  key_plus.drawButton();

  tft.setFreeFont(LABEL1_FONT);
  key_minus.initButton(&tft, 240, 30, 35, 30, TFT_WHITE, TFT_BLUE, TFT_WHITE, "-", KEY_TEXTSIZE); 
  key_minus.drawButton();

  tft.setFreeFont(LABEL1_FONT);
  key_set_auto.initButton(&tft, 200, 65, 35, 30, TFT_WHITE, TFT_YELLOW, TFT_WHITE, "set", KEY_TEXTSIZE); 
  key_set_auto.drawButton();
  */
  
  //tft.setFreeFont(FSB9);
  //tft.setTextSize(2);
  //tft.setTextColor(TFT_RED, TFT_GREEN);
  //tft.setCursor(5, 5);
  //tft.println("Temperatura: ", int(tft.width() / 4 + 10), 110, 2);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void loop()
{ 
// start of non-blocking connection setup section
  if ((WiFi.status() != WL_CONNECTED) && (conn_stat != 1)) { conn_stat = 0; }
  if ((WiFi.status() == WL_CONNECTED) && !client.connected() && (conn_stat != 3))  { conn_stat = 2; }
  if ((WiFi.status() == WL_CONNECTED) && client.connected() && (conn_stat != 5)) { conn_stat = 4;}
  switch (conn_stat) {
    case 0:                                                       // MQTT and WiFi down: start WiFi
      Serial.println("MQTT and WiFi down: start WiFi");
      tft.setCursor(5, 5);
      tft.println("MQTT and WiFi down: start WiFi");
      setup_wifi();
      conn_stat = 1;
      break;
    case 1:                                                       // WiFi starting, do nothing here
      Serial.println("WiFi starting, wait : "+ String(waitCount));
      tft.setCursor(5, 5);
      tft.println("WiFi starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 2:                                                       // WiFi up, MQTT down: start MQTT
      Serial.println("WiFi up, MQTT down: start MQTT");
      tft.setCursor(5, 5);
      tft.println("WiFi up, MQTT down: start MQTT");
      reconnect();
      conn_stat = 3;
      waitCount = 0;
      break;
    case 3:                                                       // WiFi up, MQTT starting, do nothing here
      Serial.println("WiFi up, MQTT starting, wait : "+ String(waitCount));
      tft.setCursor(5, 5);
      tft.println("WiFi up, MQTT starting, wait : "+ String(waitCount));
      waitCount++;
      break;
    case 4:                                                       // WiFi up, MQTT up: finish MQTT configuration
      Serial.println("WiFi up, MQTT up: finish MQTT configuration");
      tft.setCursor(5, 5);
      tft.println("WiFi up, MQTT up: finish MQTT configuration");
      delay(5);
      client.subscribe("Touch_screen_temp");
      client.subscribe("Touch_screen_humid");
      client.subscribe("Touch_screen_air");
      client.subscribe("Touch_screen_k_temp");
      client.subscribe("Touch_screen_k_humid");
      client.subscribe("Touch_screen_b_temp");
      client.subscribe("Touch_screen_b_humid");
      //mqttClient.publish(input_topic, Version);
      initialize_climate_menu();
      conn_stat = 5;                    
      break;
  }
// end of non-blocking connection setup section

// start section with tasks where WiFi/MQTT is required
  if (conn_stat == 5) {
    client.loop();
    uint16_t x, y;
  
    // See if there's any touch data for us
    if (tft.getTouch(&x, &y))
    {
      // Draw a block spot to show where touch was calculated to be
      //#ifdef BLACK_SPOT
      //  tft.fillCircle(x, y, 2, TFT_BLACK);
      //#endif
      if(screen_mode == "climate"){
        climate_mode(x, y);
      }
      if(screen_mode == "menu"){
        menu_mode(x, y);
      }

  
      Serial.println(SwitchOn);
    }
  } 
  // start section for tasks which should run regardless of WiFi/MQTT
  delay(100);
  // end of section for tasks which should run regardless of WiFi/MQTT
}

void initialize_climate_menu(){
  tft.fillScreen(TFT_GREEN);
  tft.setFreeFont(LABEL1_FONT);
  tft.setTextSize(1);
  tft.setTextFont(1);
  tft.setTextColor(TFT_RED, TFT_GREEN);
  // Set the rotation before we calibrate
  // Draw button (this example does not use library Button class)
  redBtn();
  redBtn2();

  tft.setFreeFont(LABEL1_FONT);
  key_plus.initButton(&tft, 200, 30, 35, 30, TFT_WHITE, TFT_RED, TFT_WHITE, "+", KEY_TEXTSIZE); 
  key_plus.drawButton();

  tft.setFreeFont(LABEL1_FONT);
  key_minus.initButton(&tft, 240, 30, 35, 30, TFT_WHITE, TFT_BLUE, TFT_WHITE, "-", KEY_TEXTSIZE); 
  key_minus.drawButton();

  tft.setFreeFont(LABEL1_FONT);
  key_set_auto.initButton(&tft, 200, 65, 35, 30, TFT_WHITE, TFT_YELLOW, TFT_WHITE, "set", KEY_TEXTSIZE); 
  key_set_auto.drawButton();

  tft.setFreeFont(LABEL1_FONT);
  key_climate_go_to_menu.initButton(&tft, 280, 65, 40, 30, TFT_WHITE, TFT_BLACK, TFT_WHITE, "menu", KEY_TEXTSIZE); 
  key_climate_go_to_menu.drawButton();
}

void initialize_main_menu(){
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(LABEL1_FONT);
  key_climate.initButton(&tft, 40, 65, 65, 50, TFT_WHITE, TFT_GREEN, TFT_WHITE, "climate", KEY_TEXTSIZE); 
  key_climate.drawButton();
}

void menu_mode(uint16_t x, uint16_t y){
  if (key_climate.contains(x, y)) {
    key_climate.press(true); // tell the button it is pressed
    Serial.println("TRUE");
    initialize_climate_menu();
    screen_mode = "climate";
  }
}

void climate_mode(uint16_t x, uint16_t y){
  if (key_climate_go_to_menu.contains(x, y)) {
    key_climate_go_to_menu.press(true); // tell the button it is pressed
    initialize_main_menu();
    screen_mode = "menu";
  }
  if (key_plus.contains(x, y)) {
    key_plus.press(true); // tell the button it is pressed
    Serial.println("TRUE");
    target_temp += 0.1;
    display_temp();
    tft.fillCircle(240, 65, 15, TFT_GREEN);
  }
  if (key_minus.contains(x, y)) {
    key_minus.press(true); // tell the button it is pressed
    Serial.println("TRUE");
    target_temp -= 0.1;
    display_temp();
    tft.fillCircle(240, 65, 15, TFT_GREEN);
  }
  if (key_set_auto.contains(x, y)) {
    key_set_auto.press(true); // tell the button it is pressed
    tft.fillCircle(240, 65, 15, TFT_RED);
    client.publish("display_auto_temp", (String(target_temp)).c_str());
  }
  
  if (SwitchOn)
  {
    if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
      if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
        Serial.println("ventilation on");
        redBtn();
        client.publish("ventilation_living_room", "4");
        delay(100);
        client.publish("ventilation_living_room", "5");
        client.loop();
      }
    }
  }
  else //Record is off (SwitchOn == false)
  {
    if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
      if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
        Serial.println("Green btn hit");
        greenBtn();
        client.publish("ventilation_living_room", "0");
        delay(100);
        client.publish("ventilation_living_room", "1");
        client.loop();
      }
    }
  }
  if (SwitchOn2)
  {
    if ((x > REDBUTTON_X2) && (x < (REDBUTTON_X2 + REDBUTTON_W2))) {
      if ((y > REDBUTTON_Y2) && (y <= (REDBUTTON_Y2 + REDBUTTON_H2))) {
        Serial.println("Red btn hit");
        const char* topic = "led_kitchen";
        set_color(0, 116, 157, 0, topic);
        client.loop();
        delay(100);
        redBtn2();
      }
    }
  }
  else
  {
    if ((x > GREENBUTTON_X2) && (x < (GREENBUTTON_X2 + GREENBUTTON_W2))) {
      if ((y > GREENBUTTON_Y2) && (y <= (GREENBUTTON_Y2 + GREENBUTTON_H2))) {
        Serial.println("Green btn hit");
        const char* topic = "led_kitchen";
        set_color(170, 116, 157, 0, topic);
        client.loop();
        greenBtn2();
      }
    }
  }
}

void reconnect() {
 // Loop until we're reconnected
   // Attempt to connect
 if (client.connect(mqtt_client_name)) {
   Serial.println("connected");
   // ... and subscribe to topic
   // client.subscribe("lamp_1");
 }
}
//------------------------------------------------------------------------------------------

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

void set_color(int i, int r, int g, int b, const char* topic) {
  // Fix note: r swipped with g. This is grb diode.
  client.publish(topic, ("i" + String(i)).c_str());
  client.publish(topic, ("r" + String(r)).c_str());
  client.publish(topic, ("g" + String(g)).c_str());
  client.publish(topic, ("b" + String(b)).c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 String mqttTemp;
 for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    mqttTemp += receivedChar;
    Serial.println(receivedChar);
  }
  String topic_receive = String(topic);
  Serial.println("topic " + topic_receive);
  if(topic_receive == "Touch_screen_temp"){
    messageTemp = mqttTemp;
  }
  if(topic_receive == "Touch_screen_humid"){
    messageHumid = mqttTemp;
  }
  if(topic_receive == "Touch_screen_air"){
    messageAir = mqttTemp;
  }
  if(topic_receive == "Touch_screen_k_temp"){
    messageTemp_k = mqttTemp;
  }
  if(topic_receive == "Touch_screen_k_humid"){
    messageHumid_k = mqttTemp;
  }
  if(topic_receive == "Touch_screen_b_temp"){
    messageTemp_b = mqttTemp;
  }
  if(topic_receive == "Touch_screen_b_humid"){
    messageHumid_b = mqttTemp;
  }
  if(screen_mode == "climate"){
    display_temp();
  }
}


void display_temp(){
  tft.setTextColor(TFT_RED, TFT_GREEN);
  tft.setTextSize(2);
  tft.setTextFont(2);
  tft.setCursor(5, 100);
  tft.println("Temp cel: " + String(target_temp));
  tft.println("Temperatura: " + messageTemp);
  tft.setTextSize(1);
  tft.setTextFont(1);
  tft.println("Wilgotnosc salon: " + messageHumid);
  tft.println("Czystosc powietrza salon: " + messageAir);
  tft.println("Temperatura kuchnia: " + messageTemp_k);
  tft.println("Wilgotnosc kuchnia: " + messageHumid_k);
  tft.println("Temperatura lazienka: " + messageTemp_b);
  tft.println("Wilgotnosc lazienka: " + messageHumid_b);
}


void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void drawFrame()
{
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}
void drawFrame2()
{
  tft.drawRect(FRAME_X2, FRAME_Y2, FRAME_W, FRAME_H, TFT_BLACK);
}

// Draw a red button
void redBtn()
{
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_RED);
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_DARKGREY);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ON vent", GREENBUTTON_X + (GREENBUTTON_W / 2), GREENBUTTON_Y + (GREENBUTTON_H / 2));
  SwitchOn = false;
}

// Draw a green button
void greenBtn()
{
  tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, TFT_GREEN);
  delay(5);
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_DARKGREY);
  delay(5);
  drawFrame();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("OFF vent", REDBUTTON_X + (REDBUTTON_W / 2) + 1, REDBUTTON_Y + (REDBUTTON_H / 2));
  SwitchOn = true;
}

// Draw a red button
void redBtn2()
{
  tft.fillRect(REDBUTTON_X2, REDBUTTON_Y2, REDBUTTON_W2, REDBUTTON_H2, TFT_RED);
  delay(5);
  tft.fillRect(GREENBUTTON_X2, GREENBUTTON_Y2, GREENBUTTON_W2, GREENBUTTON_H2, TFT_DARKGREY);
  delay(5);
  drawFrame2();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ON k led", GREENBUTTON_X2 + (GREENBUTTON_W2 / 2), GREENBUTTON_Y2 + (GREENBUTTON_H2 / 2));
  SwitchOn2 = false;
}

// Draw a green button
void greenBtn2()
{
  tft.fillRect(GREENBUTTON_X2, GREENBUTTON_Y2, GREENBUTTON_W2, GREENBUTTON_H2, TFT_GREEN);
  delay(5);
  tft.fillRect(REDBUTTON_X2, REDBUTTON_Y2, REDBUTTON_W2, REDBUTTON_H2, TFT_DARKGREY);
  delay(5);
  drawFrame2();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("OFF k led", REDBUTTON_X2 + (REDBUTTON_W2 / 2) + 1, REDBUTTON_Y2 + (REDBUTTON_H2 / 2));
  SwitchOn2 = true;
}

/*
void drawKeypad()
{
// Draw the Button
tft.setFreeFont(LABEL1_FONT);
key.initButton(&tft, // REF - LEAVE AS IS
KEY_X, // X Cord: SEE ABOVE Line 19
KEY_Y, // Y CORD: SEE ABOVE Line 20
KEY_W, // WIDTH: SEE ABOVE Line 21
KEY_H, // HEIGHT: SEE ABOVE Line 22
TFT_WHITE, // OUTLINE
keyColor, // TEXT COLOR
TFT_WHITE, // FILL
keyLabel, // TEXT TO PRINT
KEY_TEXTSIZE); // TEXT SIZE: SEE ABOVE Line 23
key.drawButton();
}
*/
