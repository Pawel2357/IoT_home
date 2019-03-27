import paho.mqtt.client as mqtt
import time

client = mqtt.Client()
client.connect("40.121.36.126", 1883)

while True:
    client.publish("sensors_3d_printing_room", "a")
    time.sleep(3)
    client.publish("sensors_3d_printing_room", "h")
    time.sleep(3)
    client.publish("sensors_3d_printing_room", "t")
    time.sleep(60)
