import paho.mqtt.client as mqtt
import time
import csv

# This is the Subscriber

filename = "/home/pawel/Documents/IoT_home/data/stairs_climate_data.csv"
broker_ip = "192.168.1.198"
soc_battery_topic = "climate_stairs"


def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(soc_battery_topic)



def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    data = str(msg.payload.decode("utf-8"))
    with open(filename, mode='a') as file_soc:


        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, data])


client = mqtt.Client()
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
