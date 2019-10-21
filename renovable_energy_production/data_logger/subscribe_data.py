import paho.mqtt.client as mqtt
import time
import csv
import numpy as np

# This is the Subscriber

filename = "battery_soc_data.txt"
broker_ip = "192.168.1.198"
soc_battery_topic = "home_battery_soc"


def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(soc_battery_topic)


def load_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish("renovable_control", "6")
    time.sleep(2)
    #client_2.publish("renovable_control", "7")
    time.sleep(2)


def load_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish("renovable_control", "e")
    time.sleep(1)
    client_2.publish("renovable_control", "f")
    time.sleep(1)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    soc = int(str(msg.payload.decode("utf-8"))[1:-1])
    print(soc)
    with open('soc_data.csv', mode='a') as file_soc:


        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, soc])
    with open("soc_data.csv") as csv_file:
        list = csv_file.readlines()[-10:]
        soc_l = []
        for row in list:
            soc = row.split(",")
            soc_l.append(int(soc[1]))
    print(soc_l)
    print(np.mean(soc_l))
    if np.mean(soc_l) > 93:
        load_on()
    if np.mean(soc_l) < 80:
        load_off()

client = mqtt.Client()
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
