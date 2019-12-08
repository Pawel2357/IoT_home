## Intro
This repository contain hardware and software needed to build smart self-sufficient home. This include
 - renovable energy storage and production
 - autonomous hydroponics garden
 - smart light
 - smart outlets
 
 Coming soon
- air quality home control
- home heating AI optimization control
- smart mirror
- autonomous vacuum cleaner
- autonomous weed removing robot

## Hardware setup
### How to set up Node MCU v3:
1. Go to Arduino IDE
2. Set board to NodeMCU 1.0
3. Connect to right port
4. Upload the sketch

### How to set up Arduino Uno3:
1. Go to Arduino IDE
2. Set board to Arduino Uno
3. Connect to right port
4. Upload the sketch

Remark:
- When uploading remove Rx and Tx conncetions.
- make sure broker IP(hostname -I) adress is correct

## Software setup
### How to install ubuntu/raspberry broker  
`sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa`  
`sudo apt-get update`  
`sudo apt-get install mosquitto`  
`sudo apt-get install mosquitto-clients`  
`pip3 install paho-mqtt` 

## Jupyter lab
`/usr/bin/python3 /home/pawel/.local/bin/jupyter-lab --NotebookApp.token= --ip=0.0.0.0`

## Useful links
https://icircuit.net/arduino-connecting-arduino-uno-esp8266/2443  
https://techtutorialsx.com/2017/04/09/esp8266-connecting-to-mqtt-broker/  
https://thingsboard.io/docs/samples/arduino/temperature/#esp8266-firmware  
https://slashposts.com/2018/04/mqtt-pubsubclient-tutorial-for-arduino-esp8266-esp32/  
https://techtutorialsx.com/2017/04/24/esp32-publishing-messages-to-mqtt-topic/  
good paho python tutorial http://www.steves-internet-guide.com/into-mqtt-python-client/  
http://www.steves-internet-guide.com/mqtt-python-callbacks/  
http://www.steves-internet-guide.com/loop-python-mqtt-client/
