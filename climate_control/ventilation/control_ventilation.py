import paho.mqtt.client as mqtt
import time
import csv
import datetime

broker_ip = "xyz"
topic_ventilation = "ventilation_living_room"


def turn_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic_ventilation, "1")
    time.sleep(2)


def turn_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic_ventilation, "0")
    time.sleep(2)

while True:
    now = datetime.datetime.now()
    if now.hour % 3 == 0:
        turn_on()
        time.sleep(10)
    else:
        turn_off()
        time.sleep(10)
