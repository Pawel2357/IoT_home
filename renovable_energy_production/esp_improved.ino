#include <EEPROM.h>
#include <ModbusMaster.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SoftwareSerial.h>


// Connect to the WiFi
const char* ssid =        "xyz";
const char* password =    "xyz";
const char* mqtt_server = "xyz";
uint16_t sleepSeconds =    120;         // 2 minutes default
char* MQTT_client =       "home_battery_soc";
char* data_topic =        "home_battery_soc";
const char* topic_subscribe = "renovable_control";
char* log_topic =          "home_log";

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

String a_option = "y";
String inverter_status = "n"; // means the inverter works normally, n means inverter is turned off
int case_send = 0;


WiFiClient espClient;
PubSubClient client(espClient);



// Pins
//
#define LED             D4  // for flashing the led - LOW active!
#define MAX485_DE       D2  // data or
#define MAX485_RE       D1  //      recv enable


// ModBus Register Locations
//
#define LIVE_DATA       0x3100     // start of live-data 
#define LIVE_DATA_CNT   16         // 16 regs

// just for reference, not used in code
#define PANEL_VOLTS     0x00       
#define PANEL_AMPS      0x01
#define PANEL_POWER_L   0x02
#define PANEL_POWER_H   0x03

#define BATT_VOLTS      0x04
#define BATT_AMPS       0x05
#define BATT_POWER_L    0x06
#define BATT_POWER_H    0x07

// dummy * 4

#define LOAD_VOLTS      0x0C
#define LOAD_AMPS       0x0D
#define LOAD_POWER_L    0x0E
#define LOAD_POWER_H    0x0F



#define RTC_CLOCK           0x9013  // D7-0 Sec, D15-8 Min  :   D7-0 Hour, D15-8 Day  :  D7-0 Month, D15-8 Year
#define RTC_CLOCK_CNT       3       // 3 regs

#define BATTERY_SOC         0x311A  // State of Charge in percent, 1 reg

#define BATTERY_CURRENT_L   0x331B  // Battery current L
#define BATTERY_CURRENT_H   0x331C  // Battery current H



#define STATISTICS      0x3300 // start of statistical data
#define STATISTICS_CNT  22     // 22 regs

// just for reference, not used in code
#define PV_MAX     0x00 // Maximum input volt (PV) today  
#define PV_MIN     0x01 // Minimum input volt (PV) today
#define BATT_MAX   0x02 // Maximum battery volt today
#define BATT_MIN   0x03 // Minimum battery volt today

#define CONS_ENERGY_DAY_L   0x04 // Consumed energy today L
#define CONS_ENGERY_DAY_H   0x05 // Consumed energy today H
#define CONS_ENGERY_MON_L   0x06 // Consumed energy this month L 
#define CONS_ENGERY_MON_H   0x07 // Consumed energy this month H
#define CONS_ENGERY_YEAR_L  0x08 // Consumed energy this year L
#define CONS_ENGERY_YEAR_H  0x09 // Consumed energy this year H
#define CONS_ENGERY_TOT_L   0x0A // Total consumed energy L
#define CONS_ENGERY_TOT_H   0x0B // Total consumed energy  H

#define GEN_ENERGY_DAY_L   0x0C // Generated energy today L
#define GEN_ENERGY_DAY_H   0x0D // Generated energy today H
#define GEN_ENERGY_MON_L   0x0E // Generated energy this month L
#define GEN_ENERGY_MON_H   0x0F // Generated energy this month H
#define GEN_ENERGY_YEAR_L  0x10 // Generated energy this year L
#define GEN_ENERGY_YEAR_H  0x11 // Generated energy this year H
#define GEN_ENERGY_TOT_L   0x12 // Total generated energy L
#define GEN_ENERGY_TOT_H   0x13 // Total Generated energy  H

#define CO2_REDUCTION_L    0x14 // Carbon dioxide reduction L  
#define CO2_REDUCTION_H    0x15 // Carbon dioxide reduction H 


#define LOAD_STATE         0x02 // r/w load switch state

#define STATUS_FLAGS    0x3200
#define STATUS_BATTERY    0x00  // Battery status register
#define STATUS_CHARGER    0x01  // Charging equipment status register



ModbusMaster node;   // instantiate ModbusMaster object

//WiFiClient wifi_client;
//PubSubClient mqtt_client(wifi_client);

char mqtt_msg[64];
char buf[256];
int do_update = 0, switch_load = 0;
bool loadState = true;
int debug_mode = 0;             // no sleep and mmore updates
int last_bSOC = 0;
SoftwareSerial s_relay(D5,D6); // (Rx, Tx)


void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    //Serial.println();
    //Serial.print("Connecting to ");
    //Serial.println(ssid);

    //WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
}

void setup_solar_charger() {
    // init modbus in receive mode
    pinMode(MAX485_RE, OUTPUT);
    pinMode(MAX485_DE, OUTPUT);
    digitalWrite(MAX485_RE, 0);
    digitalWrite(MAX485_DE, 0);
    // EPEver Device ID 1
    node.begin(1, Serial);
    // modbus callbacks
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
    // Initialize the LED_BUILTIN pin as an output, low active
    pinMode(LED, OUTPUT); 
    digitalWrite(LED, HIGH);
}



void get_charger_data(String *bSOC, String *live_l_bV, String *data) {
// datastructures, also for buffer to values conversion
  //
  uint8_t i, result;
  
  // clock
  union {
    struct {
    uint8_t  s;
    uint8_t  m;
    uint8_t  h;
    uint8_t  d;
    uint8_t  M;
    uint8_t  y;  
   } r;
    uint16_t buf[3];
  } rtc ;


  // live data
  union {
    struct {

      int16_t  pV;
      int16_t  pI;
      int32_t  pP;
   
      int16_t  bV;
      int16_t  bI;
      int32_t  bP;
      
      uint16_t  dummy[4];
      
      int16_t  lV;
      int16_t  lI;
      int32_t  lP; 

    } l;
    uint16_t  buf[16];
  } live;


  // statistics
  union {
    struct {
  
      // 4*1 = 4
      uint16_t  pVmax;
      uint16_t  pVmin;
      uint16_t  bVmax;
      uint16_t  bVmin;
  
      // 4*2 = 8
      uint32_t  consEnerDay;
      uint32_t  consEnerMon;
      uint32_t  consEnerYear;
      uint32_t  consEnerTotal;
  
      // 4*2 = 8
      uint32_t  genEnerDay;
      uint32_t  genEnerMon;
      uint32_t  genEnerYear;
      uint32_t  genEnerTotal;
  
      // 1*2 = 2
      uint32_t  c02Reduction;
     
    } s;
    uint16_t  buf[22];  
  } stats;

  // these are too far away for the union conversion trick
  uint16_t batterySOC = 0;
  int32_t batteryCurrent = 0;

  // battery status
  struct {
    uint8_t volt;        // d3-d0  Voltage:     00H Normal, 01H Overvolt, 02H UnderVolt, 03H Low Volt Disconnect, 04H Fault
    uint8_t temp;        // d7-d4  Temperature: 00H Normal, 01H Over warning settings, 02H Lower than the warning settings
    uint8_t resistance;  // d8     abnormal 1, normal 0
    uint8_t rated_volt;  // d15    1-Wrong identification for rated voltage
  } status_batt ;

  char batt_volt_status[][20] = {
    "Normal",
    "Overvolt",
    "Low Volt Disconnect",
    "Fault"
  };
  
  char batt_temp_status[][16] = {
    "Normal",
    "Over WarnTemp",
    "Below WarnTemp"
  };

  // charging equipment status (not fully impl. yet)
  //uint8_t charger_operation = 0;
  //uint8_t charger_state = 0;
  //uint8_t charger_input = 0;
  uint8_t charger_mode  = 0;

  //char charger_input_status[][20] = {
  //  "Normal",
  //  "No power connected",
  //  "Higher volt input",
  //  "Input volt error"
  //};
    
  char charger_charging_status[][12] = {
    "Off",
    "Float",
    "Boost",
    "Equlization"
  };

   // flash the led
   for(i=0; i<3; i++){
     digitalWrite(LED, LOW);
     delay(200);
     digitalWrite(LED, HIGH);
     delay(200);
   }

  // clear old data
  //
  memset(rtc.buf,0,sizeof(rtc.buf));
  memset(live.buf,0,sizeof(live.buf));
  memset(stats.buf,0,sizeof(stats.buf));

  // Read registers for clock
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readHoldingRegisters(RTC_CLOCK, RTC_CLOCK_CNT);
  if (result == node.ku8MBSuccess)  {

    rtc.buf[0]  = node.getResponseBuffer(0);
    rtc.buf[1]  = node.getResponseBuffer(1);
    rtc.buf[2]  = node.getResponseBuffer(2);
    
  } else {
    //Serial.print("Miss read rtc-data, ret val:");
    //Serial.println(result, HEX);
  } 

  // read LIVE-Data
  // 
  delay(200);
  node.clearResponseBuffer();
  result = node.readInputRegisters(LIVE_DATA, LIVE_DATA_CNT);

  if (result == node.ku8MBSuccess)  {

    for(i=0; i< LIVE_DATA_CNT ;i++) live.buf[i] = node.getResponseBuffer(i);
       
  } else {
    //Serial.print("Miss read liva-data, ret val:");
    //Serial.println(result, HEX);
  } 

  // Statistical Data
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readInputRegisters(STATISTICS, STATISTICS_CNT);

  if (result == node.ku8MBSuccess)  {
    
    for(i=0; i< STATISTICS_CNT ;i++)  stats.buf[i] = node.getResponseBuffer(i);
    
  } else  {
    //Serial.print("Miss read statistics, ret val:");
    //Serial.println(result, HEX);
  } 

  // Battery SOC
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readInputRegisters(BATTERY_SOC, 1);
  if (result == node.ku8MBSuccess)  {
    
    batterySOC = node.getResponseBuffer(0);
    
  } else  {
    //Serial.print("Miss read batterySOC, ret val:");
    //Serial.println(result, HEX);
  }

  // Battery Net Current = Icharge - Iload
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readInputRegisters(  BATTERY_CURRENT_L, 2);
  if (result == node.ku8MBSuccess)  {
    
    batteryCurrent = node.getResponseBuffer(0);
    batteryCurrent |= node.getResponseBuffer(1) << 16;
    
  } else  {
    //Serial.print("Miss read batteryCurrent, ret val:");
    //Serial.println(result, HEX);
  }

  // State of the Load Switch
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readCoils(  LOAD_STATE, 1 );
  if (result == node.ku8MBSuccess)  {
    
    loadState = node.getResponseBuffer(0);
        
  } else  {
    //Serial.print("Miss read loadState, ret val:");
    //Serial.println(result, HEX);
  }
    
  // Read Status Flags
  //
  delay(200);
  node.clearResponseBuffer();
  result = node.readInputRegisters(  0x3200, 2 );
  if (result == node.ku8MBSuccess)  {

    uint16_t temp = node.getResponseBuffer(0);
    //Serial.print( "Batt Flags : "); Serial.println(temp);
    
    status_batt.volt = temp & 0b1111;
    status_batt.temp = (temp  >> 4 ) & 0b1111;
    status_batt.resistance = (temp  >>  8 ) & 0b1;
    status_batt.rated_volt = (temp  >> 15 ) & 0b1;

    temp = node.getResponseBuffer(1);
    //Serial.print( "Chrg Flags : "); Serial.println(temp, HEX);

    //for(i=0; i<16; i++) Serial.print( (temp >> (15-i) ) & 1 );
    //Serial.println();
    
    //charger_input     = ( temp & 0b0000000000000000 ) >> 15 ;
    charger_mode        = ( temp & 0b0000000000001100 ) >> 2 ;
    //charger_input     = ( temp & 0b0000000000000000 ) >> 12 ;
    //charger_operation = ( temp & 0b0000000000000000 ) >> 0 ;
    
    //Serial.print( "charger_input : "); Serial.println( charger_input );
    //Serial.print( "charger_mode  : "); Serial.println( charger_mode );
    //Serial.print( "charger_oper  : "); Serial.println( charger_operation );
    //Serial.print( "charger_state : "); Serial.println( charger_state );
    
  } else  {
    //Serial.print("Miss read ChargeState, ret val:");
    //Serial.println(result, HEX);
  }
  
  // results
  //String soc = "," + String(batterySOC) + ",";
  //Serial.print("batterySOC" + String(batterySOC));
  //publish_data(soc);
  //delay(20000);
  
  Serial.print("batterySOC" + String(batterySOC));
  Serial.print("batteryCurrent" + String(batteryCurrent));
  Serial.print("live.l.pV" + String(live.l.pV));
  Serial.print("live.l.pI" + String(live.l.pI));
  Serial.print("live.l.pP" + String(live.l.pP));
  Serial.print("live.l.bV" + String(live.l.bV));
  Serial.print("live.l.bI" + String(live.l.bI));
  Serial.print("live.l.bP" + String(live.l.bP));
  Serial.print("charger_charging_status[ charger_mode]" + String(charger_charging_status[ charger_mode]));
  Serial.print("stats.s.genEnerDay" + String(stats.s.genEnerDay));
  Serial.print("stats.s.genEnerTotal" + String(stats.s.genEnerTotal));   
  *data = String(batterySOC) + "," + String(batteryCurrent) + "," + String(live.l.pV) + "," + String(live.l.pI) + "," + String(live.l.pP) + "," + String(live.l.bV) + "," + String(live.l.bI) + "," + String(live.l.bP) + "," + String(charger_charging_status[ charger_mode]) + "," + String(stats.s.genEnerDay) + "," + String(stats.s.genEnerTotal);
  *live_l_bV = String(live.l.bV);
  *bSOC = String(batterySOC);
}

void send_to_arduino(String receivedChar){
  //char sm[] = receivedChar;
  s_relay.write(receivedChar[0]);
}

void callback(char* topic, byte* payload, unsigned int length) {
 // get subscribed message char by char
 // TODO: Do it separately for different topics or add time distance between got chars to communicate separately.
 String sign = "";
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];

  sign += receivedChar;
  Serial.print("sign");
  Serial.print(sign);
  if(sign == "n"){
    a_option="n";
  }else if(sign == "y"){
    a_option="y";
  }
  else{
    send_to_arduino(sign);
  }
  
  /*
  if (sign == "f"){ //  inverter_status
    inverter_status = "n";
    send_to_arduino("a");
    delay(40);
    send_to_arduino("b");
    delay(40);
    send_to_arduino("c");
    delay(40);
    send_to_arduino("d");
    delay(40);
    send_to_arduino("e");
    delay(40);
    send_to_arduino("g");
    delay(40);
    send_to_arduino("f");
  }
  if (sign == "7"){
    inverter_status = "o";
    send_to_arduino("7");
  }
  if (inverter_status == "o"){
    delay(60);
    send_to_arduino(sign);
  } */
  
  }
}

void setup(){
    // say hello
    Serial.begin(115200);
    s_relay.begin(9600);
    WiFi.mode(WIFI_STA);
    while (!Serial) { ; }
    //Serial.println();
    //Serial.println("Hello World! I'm an EpEver Solar Monitor!");
    client.setCallback(callback);

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    
    setup_solar_charger();
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

void preTransmission()
{
  digitalWrite(MAX485_RE, 1);
  digitalWrite(MAX485_DE, 1);
  
  digitalWrite(LED, LOW);
}

void postTransmission()
{
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);
  
  digitalWrite(LED,HIGH);
}


void loop(){

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
      //Serial.println("WiFi up, MQTT down: start MQTT");
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
  if (conn_stat == 5) {
    if (millis() - lastStatus > 20000) {                            // Start send status every 10 sec (just as an example)
      String bSOC;
      String live_l_bV;
      String data;
      get_charger_data(&bSOC, &live_l_bV, &data);
      client.publish(data_topic, String(data).c_str(), true);              //      send status to broker
      client.loop();                                            //      give control to MQTT to send message to broker
      lastStatus = millis();                                        //      remember time of last sent status message
    }                                           // internal household function for OTA
    String data;
    if (s_relay.available()>0)
      {
        data=s_relay.readString();
        Serial.println("e use" + data);
        client.publish("energy_use", String(data).c_str(), true);
      }
    client.loop();                                              // internal household function for MQTT
  } 
// end of section for tasks where WiFi/MQTT are required

// start section for tasks which should run regardless of WiFi/MQTT
  if (waitCount > 600){
    ESP.restart();
  }
  if (millis() - lastTask > 8000) {                                 // Print message every second (just as an example)
    String bSOC;
    String live_l_bV;
    String data;
    get_charger_data(&bSOC, &live_l_bV, &data);
    float bSOC_average = (last_bSOC + bSOC.toInt()) / 2;
    float bV = live_l_bV.toFloat() / 100;
    if((bSOC_average > 95) && (a_option=="y")){
      if(case_send == 0){
        delay(40);
        send_to_arduino("7");
        case_send = 1;
      }else if(case_send == 1){
        delay(40);
        send_to_arduino("2"); // turn on car charging
        case_send = 2;
      }else if(case_send == 2){
        delay(40);
        send_to_arduino("6"); // turn on comp
        case_send = 3;
      }else if(case_send == 3){
        delay(40);
        send_to_arduino("4"); // turn on comp
        case_send = 0;
      }
    }
    Serial.println("battery v " + String(bV));
    if(bV < 43.8 && bV > 35.0){
      send_to_arduino("f");
    }
    last_bSOC = bSOC.toInt();
    lastTask = millis();
  }
  delay(100);
}


