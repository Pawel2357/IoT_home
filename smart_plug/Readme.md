## Nodemcu v3
### Arduino IDE settings
- board NodeMCU 1.0
- upload speed 115200
### Parts used in the plug
- NodeMCU v3
- digital logic converter 3.3-5v
- arduino uno/mini pro
- power supply 5v
- relay 5v
- usb-to-ttl to program arduino mini pro
### Connections
#### arduino - relay
Connect relay in to pin 13 arduino
#### Connection arduino-nodemcu using digital logic converter
arduino - nodemcu<br/>
tx - rx<br/>
rx - tx<br/>
gnd - gnd<br/>
_ - 3.3v<br/>
#### Program arduino mini-pro
arduino - usb-to-ttl
tx - rx<br/>
gnd - gnd<br/>
Vin - 5v<br/>

#### Connect more then one client to the same broker
- make sure each client has different client_id

## ESP-01:
### Connections:
ESP, usb_to_ttl<br/>
RX - TX<br/>
TX - RX<br/>
3.3 v - 3.3 v(not from usb to ttl, but from high current source(arduino uno))<br/>
EN - 3.3 v(not from usb to ttl, but from high current source(arduino uno))<br/>
GND - GND(usb to ttl needs to be grounded with esp and other current 3.3v source<br/>
GPIO 0 - 1k ohm - GND<br/>

#### Uploading instruction
1. First insert usb to the pc
2. Power the esp

### Arduino IDE
1. Set board to Generic ESP8266 module
2. Set upload speed to 115200
