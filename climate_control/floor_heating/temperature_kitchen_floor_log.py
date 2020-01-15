import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import config as cfg
import paho.mqtt.client as mqtt
import time
import csv

# This is the Subscriber

filename = cfg.floor_temp_data
broker_ip = cfg.broker_ip
broker_port = cfg.broker_port
topic = cfg.kitchen_floor_temp_log_topic


def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(topic)



def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    data = str(msg.payload.decode("utf-8"))
    with open(filename, mode='a') as file_soc:


        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, data])


client = mqtt.Client()
client.connect(broker_ip, broker_port)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
