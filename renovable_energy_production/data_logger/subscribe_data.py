import paho.mqtt.client as mqtt
import time
import csv

# This is the Subscriber

filename = "battery_soc_data.txt"
broker_ip = "xxx"
soc_battery_topic = "home_battery_soc"


def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(soc_battery_topic)


def load_on(client):
    client.publish("renovable_control", "6")
    time.sleep(2)

    client.publish("renovable_control", "7")
    time.sleep(2)


def load_off(client):
    client.publish("renovable_control", "e")
    time.sleep(2)
    client.publish("renovable_control", "f")
    time.sleep(2)


def on_message(client, userdata, msg):
    localtime = time.asctime(time.localtime(time.time()))
    with open('soc_data.csv', mode='wa') as file_soc:
        soc = str(msg.payload.decode("utf-8"))

        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, soc])
        if soc >= 90:
            load_on(client)
        if soc <= 75:
            load_off(client)


client = mqtt.Client()
client.connect(broker_ip, 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
