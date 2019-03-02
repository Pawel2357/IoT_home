import paho.mqtt.publish as publish
import time
import paho.mqtt.client as mqtt

broker_adress = "192.168.0.120"
client = mqtt.Client("P1")
client.connect(broker_adress)
print("Sending 0...")
client.publish("ledStatus", "0")
time.sleep(3)
print("Sending 1...")
client.publish("ledStatus", "1")

def on_message(client, userdata, message):
     print("message", str(message.payload.decode("utf-8")))
     print("message topic", message.topic)

client.on_message = on_message
client.loop_start()
client.subscribe("test")
client.publish("test", "OFF")
time.sleep(4)
client.loop_stop()