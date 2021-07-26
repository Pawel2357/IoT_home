import paho.mqtt.client as mqtt
import time

# Importing IntentBuilder
from adapt.intent import IntentBuilder
# Importing MycroftSkill class
from mycroft.skills.core import MycroftSkill

topic_charger = "charging_1"

# Creating HelloWorldSKill extending MycroftSkill
class OnCharging(MycroftSkill):
    
    def __init__(self):
        super(OnCharging, self).__init__("OnCharging")

    def initialize(self):
        # Creating GreetingsIntent requiring Ventilation vocab
        charging = IntentBuilder("ChargingIntent").require("Charger").build()
        # Associating a callback with the Intent
        self.register_intent(charging, self.handle_charging)
        
    def handle_charging(self):
        broker_ip = "192.168.1.198"
        broker_port = 1883
        client_3 = mqtt.Client()
        client_3.connect(broker_ip, broker_port)
        client_3.publish(topic_charger, "4")
        # Sending a command to mycroft, speak Greetings Dialog
        self.speak_dialog("Charger")
        
    def stop(self):
        pass


def create_skill():
    return OnCharging()