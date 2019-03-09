import paho.mqtt.publish as publish
import time
import paho.mqtt.client as mqtt

broker_adress = "192.168.0.120"
client = mqtt.Client("P1")
client.connect(broker_adress)
print("Sending 0...")
client.publish("sensors_3d_printing_room", "0")
time.sleep(3)
print("Sending 1...")
client.publish("sensors_3d_printing_room", "1")
time.sleep(4)

def on_message(client, userdata, message):
     print("message", str(message.payload.decode("utf-8")))
     with open("first_data.txt", "a") as myfile:
         myfile.write(message.topic + "," +str(message.payload.decode("utf-8")))
     print("message topic", message.topic)

while True:
	client.on_message = on_message
	client.loop_start()
	client.subscribe("test")
	client.publish("sensors_3d_printing_room", "a")
	time.sleep(4)
	client.publish("sensors_3d_printing_room", "t")
	time.sleep(4)
	client.publish("sensors_3d_printing_room", "h")
	time.sleep(4)
	client.loop_stop()
