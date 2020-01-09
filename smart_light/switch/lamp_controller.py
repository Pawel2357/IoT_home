import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import config as cfg
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
import csv
import datetime
import numpy as np

# This is the Subscriber

broker_ip = cfg.broker_ip
broker_port = cfg.broker_port
topic_receive = cfg.topic_lamp_controller
topic_send = [cfg.topic_lamp_bathroom_1, cfg.topic_lamp_bathroom_2]

now = datetime.datetime.now()

def change_color(r, g, b, intensity, topic):
    if r != '':
        publish.single(topic, "r" + str(r), hostname=broker_ip)
    time.sleep(0.05)
    if g != '':
        publish.single(topic, "g" + str(g), hostname=broker_ip)
    time.sleep(0.05)
    if b != '':
        publish.single(topic, "b" + str(b), hostname=broker_ip)
    time.sleep(0.05)
    if intensity != '':
        publish.single(topic, "i" + str(intensity), hostname=broker_ip)


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    command = str(msg.payload.decode("utf-8"))
    print(int(command))
    if int(command) > 0 and int(command) < 5:
        change_color(255, 255, 255, 255, topic_send[int(command)])

client = mqtt.Client(topic_receive)
client.connect(broker_ip, broker_port)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
