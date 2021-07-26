import paho.mqtt.client as mqtt
import time

# Importing IntentBuilder
from adapt.intent import IntentBuilder
# Importing MycroftSkill class
from mycroft.skills.core import MycroftSkill


def slow_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "0")
    time.sleep(0.05)
    client.publish(topic, "5")
    
def fast_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "0")
    time.sleep(0.05)
    client.publish(topic, "1")
    
def off_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "4")
    time.sleep(0.05)
    client.publish(topic, "5")


# Creating HelloWorldSKill extending MycroftSkill
class DayVentilation(MycroftSkill):
    
    def __init__(self):
        super(DayVentilation, self).__init__("DayVentilation")

    def initialize(self):
        # Creating GreetingsIntent requiring Ventilation vocab
        ventilation = IntentBuilder("Ventilation_day_Intent").require("Ventilation_day").build()
        # Associating a callback with the Intent
        self.register_intent(ventilation, self.handle_greetings)
        
    def handle_greetings(self):
        topic_ventilation = "ventilation_living_room"
        topic_ventilation_bedroom = "ventilation_bedroom"
        broker_ip = "192.168.1.198"
        broker_port = 1883
        client_3 = mqtt.Client()
        client_3.connect(broker_ip, broker_port)
        slow_ventilation(client_3, topic_ventilation)
        off_ventilation(client_3, topic_ventilation_bedroom)
        # Sending a command to mycroft, speak Greetings Dialog
        self.speak_dialog("Ventilation_day")
        
    def stop(self):
        pass


def create_skill():
    return DayVentilation()
