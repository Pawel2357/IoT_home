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

#include <WiFi.h>
#include <PubSubClient.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "Dom_2_4";
const char* password = "izabelin";
const char* mqtt_server = "192.168.1.198";

WiFiClient espClient;
PubSubClient client(espClient);

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData6"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

boolean SwitchOn = false;
boolean SwitchOn2 = false;
String messageTemp;

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


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(9600);
  tft.init();
  tft.fillScreen(TFT_GREEN);
  // Set the rotation before we calibrate
  tft.setRotation(1);

  // call screen calibration
  touch_calibrate();

  // clear screen

  // Draw button (this example does not use library Button class)
  redBtn();
  redBtn2();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(500);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void loop()
{ 
  if ((!client.connected()) && (WiFi.status() != WL_CONNECTED)) {
    setup_wifi();
  }else{
    if (!client.connected()){
      reconnect();
    }else{
      client.loop();
      uint16_t x, y;
    
      // See if there's any touch data for us
      if (tft.getTouch(&x, &y))
      {
        // Draw a block spot to show where touch was calculated to be
        //#ifdef BLACK_SPOT
        //  tft.fillCircle(x, y, 2, TFT_BLACK);
        //#endif
        
        if (SwitchOn)
        {
          if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
            if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
              Serial.println("ventilation on");
              redBtn();
              client.publish("ventilation_living_room", "4");
              delay(100);
              client.publish("ventilation_living_room", "5");
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
            }
          }
        }
        if (SwitchOn2)
        {
          if ((x > REDBUTTON_X2) && (x < (REDBUTTON_X2 + REDBUTTON_W2))) {
            if ((y > REDBUTTON_Y2) && (y <= (REDBUTTON_Y2 + REDBUTTON_H2))) {
              Serial.println("Red btn hit");
              const char* topic = "led_kitchen";
              set_color(0, 255, 0, 0, topic);
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
              set_color(170, 255, 0, 0, topic);
              greenBtn2();
            }
          }
        }
    
        Serial.println(SwitchOn);
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("touch_screen")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Touch_screen_temp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//------------------------------------------------------------------------------------------

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
 String messageTemp;
 messageTemp = "";
 for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    messageTemp += receivedChar;
    Serial.println(receivedChar);
  }
  tft.setTextColor(TFT_RED, TFT_GREEN);
  tft.drawString(messageTemp, 160, 130, 6);
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
  tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, TFT_DARKGREY);
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
  tft.fillRect(GREENBUTTON_X2, GREENBUTTON_Y2, GREENBUTTON_W2, GREENBUTTON_H2, TFT_DARKGREY);
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
  tft.fillRect(REDBUTTON_X2, REDBUTTON_Y2, REDBUTTON_W2, REDBUTTON_H2, TFT_DARKGREY);
  drawFrame2();
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("OFF k led", REDBUTTON_X2 + (REDBUTTON_W2 / 2) + 1, REDBUTTON_Y2 + (REDBUTTON_H2 / 2));
  SwitchOn2 = true;
}
