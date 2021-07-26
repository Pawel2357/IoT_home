import paho.mqtt.client as mqtt
import time

# Importing IntentBuilder
from adapt.intent import IntentBuilder
# Importing MycroftSkill class
from mycroft.skills.core import MycroftSkill

bathroom_topic_1 = "lamp_1"
bathroom_topic_2 = "lamp_2"
bathroom_topic_3 = "lamp_3"

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

# Creating HelloWorldSKill extending MycroftSkill
class BathroomNightLight(MycroftSkill):
    
    def __init__(self):
        super(BathroomNightLight, self).__init__("BathroomNightLight")

    def initialize(self):
        # Creating GreetingsIntent requiring Ventilation vocab
        charging = IntentBuilder("LightIntent").require("Light").build()
        # Associating a callback with the Intent
        self.register_intent(charging, self.handle_charging)
        
    def handle_charging(self):
        broker_ip = "192.168.1.198"
        broker_port = 1883
        client_3 = mqtt.Client()
        client_3.connect(broker_ip, broker_port)
        r = 45
        g = 157
        b = 0
        i = 5
        change_color(client_3, r, g, b, i, bathroom_topic_1)
        change_color(client_3, r, g, b, i, bathroom_topic_2)
        change_color(client_3, r, g, b, i, bathroom_topic_3)
        # Sending a command to mycroft, speak Greetings Dialog
        self.speak_dialog("Light")
        
    def stop(self):
        pass


def create_skill():
    return BathroomNightLight()