import paho.mqtt.client as mqtt    #import client library
import time
port=1883
broker_adress = "40.121.36.126"
client = mqtt.Client("P1")
client.connect(broker_adress, port)
client.publish("smart_leaf_gps", "test")
