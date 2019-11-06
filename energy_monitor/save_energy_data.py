import paho.mqtt.client as mqtt
import time
import csv
import numpy as np

# This is the Subscriber

filename = 'power_7_data.csv'
broker_ip = "0.0.0.0"
soc_battery_topic = "energy_monitor"


def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(soc_battery_topic)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    power = str(msg.payload.decode("utf-8"))
    print(power)
    with open(filename, mode='a') as file_soc:
        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, power])

client = mqtt.Client()
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
