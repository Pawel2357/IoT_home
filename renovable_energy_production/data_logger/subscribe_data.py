import paho.mqtt.client as mqtt
import time

# This is the Subscriber

filename = "battery_soc_data.txt"
broker_ip = "xxx"
soc_battery_topic = "home_battery_soc"

def save_to_file(filename, message):
     with open(filename, "a") as myfile:
         myfile.write(message)

def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(soc_battery_topic)

def on_message(client, userdata, msg):
  localtime = time.asctime(time.localtime(time.time()))
  data = msg.topic + "," + str(msg.payload.decode("utf-8")) + "," + localtime
  print(data)
  save_to_file(filename, data)

client = mqtt.Client()
client.connect("40.121.36.126", 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
