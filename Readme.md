How to set up Node MCU v3:
1. Go to Arduino IDE
2. Set board to NodeMCU 1.0
3. Connect to right port
4. Upload the sketch node_mcu_code.ino

How to set up Arduino Uno:
1. Go to Arduino IDE
2. Set board to Arduino Uno
3. Connect to right port
4. Upload the sketch arduino_uno_pin_control.ino

Remark:
- When uploading remove Rx and Tx conncetions.
- make sure broker IP(hostname -I) adress is correct


How to set up linux broker
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt-get update
sudo apt-get install mosquitto
sudo apt-get install mosquitto-clients
pip3 install paho-mqtt
