import BlynkLib
import paho.mqtt.client as mqtt
import time

def change_color(client, r, g, b, intensity, topic):
    if r != '':
        client.publish(topic, "r" + str(r))
    time.sleep(0.05)
    if g != '':
        client.publish(topic, "g" + str(g))
    time.sleep(0.05)
    if b != '':
        client.publish(topic, "b" + str(b))
    time.sleep(0.05)
    if intensity != '':
        client.publish(topic, "i" + str(intensity))
        
        
# Initialize Blynk
blynk = BlynkLib.Blynk('passw')

broker_IP = "192.168.1.198"
bathroom_topic_1 = "lamp_1"
bathroom_topic_2 = "lamp_2"
bathroom_topic_3 = "lamp_3"

r = 255
g = 255
b = 255


# Register Virtual Pins
@blynk.VIRTUAL_WRITE(0)
def my_write_handler(value):
    client = mqtt.Client()
    client.connect(broker_IP, 1883)
    r = int(value[0])
    g = int(value[1])
    b = int(value[2])
    print(r, g, b)
    change_color(client, r, g, b, 255, bathroom_topic_1)
    change_color(client, r, g, b, 255, bathroom_topic_2)
    change_color(client, r, g, b, 255, bathroom_topic_3)

while True:
    blynk.run()
