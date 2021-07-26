import paho.mqtt.client as mqtt
import time

# Importing IntentBuilder
from adapt.intent import IntentBuilder
# Importing MycroftSkill class
from mycroft.skills.core import MycroftSkill


def off_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "4")
    time.sleep(0.05)
    client.publish(topic, "5")


# Creating HelloWorldSKill extending MycroftSkill
class OffLivingRoomVentilation(MycroftSkill):
    
    def __init__(self):
        super(OffLivingRoomVentilation, self).__init__("OffLivingRoomVentilation")

    def initialize(self):
        # Creating GreetingsIntent requiring Ventilation vocab
        ventilation = IntentBuilder("VentilationIntent").require("Ventilation").build()
        # Associating a callback with the Intent
        self.register_intent(ventilation, self.handle_greetings)
        
    def handle_greetings(self):
        topic_ventilation = "ventilation_living_room"
        broker_ip = "192.168.1.198"
        broker_port = 1883
        client_3 = mqtt.Client()
        client_3.connect(broker_ip, broker_port)
        off_ventilation(client_3, topic_ventilation)
        # Sending a command to mycroft, speak Greetings Dialog
        self.speak_dialog("Ventilation")
        
    def stop(self):
        pass


def create_skill():
    return OffLivingRoomVentilation()
