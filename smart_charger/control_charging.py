import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
import csv
import datetime
import numpy as np

# This is the Subscriber

broker_ip = "zzz"
topic = "charging_1"
charging_time = 20

now = datetime.datetime.now()


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    command = str(msg.payload.decode("utf-8"))
    print(int(command))
    if int(command) == 4:
        time.sleep(charging_time * 60)
        publish.single(topic, "0", hostname=broker_ip)

client = mqtt.Client("test_1")
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
