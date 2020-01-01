import paho.mqtt.client as mqtt
import time
import csv
import numpy as np

# This is the Subscriber

broker_ip = "0.0.0.0"
topic = "charging_1"
charging_time = 20


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    command = str(msg.payload.decode("utf-8"))
    print(command)
    if command == '4':
        time.sleep(charging_time * 60)
        client = mqtt.Client()
        client.connect(broker_IP, 1883)
        client.publish(topic, '0')

client = mqtt.Client()
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
