import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import time
import datetime
import numpy as np
import config as cfg
import paho.mqtt.client as mqtt

broker_ip = cfg.broker_ip
broker_port = cfg.broker_port
topic_circulation = "circulation_pump"
topic_living_room_radiator = "radiator_living_room"

climate_data_file = '/home/pawel/Documents/IoT_home/data/climate_data.csv'

def turn_circulation_on(client, topic):
    print("circulation on")
    client.publish(topic, "1")
    
def turn_circulation_off(client, topic):
    print("circulation off")
    client.publish(topic, "0")
    
def turn_on_slow_radiator_ventilation(client, topic):
    client.publish(topic, "4")

def turn_on_fast_radiator_ventilation(client, topic):
    client.publish(topic, "9")
    
def turn_off_radiator_ventilation(client, topic):
    client.publish(topic, "0")
    

def get_temp(climate_data_file):
    with open(climate_data_file) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        for row in last10:
            x = row.split(",")
            temp.append(float(x[2]))
    temp = np.mean(temp)
    return temp

def get_target_temperature():
    now = datetime.datetime.now()
    hour = now.hour
    if hour > 4 and hour <= 5:
        return 20.0, 0
    elif hour > 5 and hour <= 7:
        return 20.2, 1
    elif hour > 7 and hour <= 8:
        return 20.4, 2
    elif hour > 8 and hour <= 10:
        return 20.9, 2
    elif hour > 10 and hour <= 15:
        return 21.0, 3
    elif hour > 15 and hour <= 18:
        return 20.7, 4
    elif hour > 18 and hour <= 21:
        return 20.77, 5
    elif hour > 21 and hour <= 4:
        return 19, 6
    else:
        return 20, 4

while True:
    time.sleep(120) # sleep for a minute 
    current_temp = get_temp(climate_data_file)
    target_temp, period_type = get_target_temperature()
    print("current_temp", current_temp)
    print("target_temp", target_temp)
    print("period_type", period_type)
    delta = 0.15
    if current_temp < target_temp - delta:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_on(client, topic_circulation)
        if period_type == 0:
            client = mqtt.Client()
            client.connect(broker_ip, broker_port)
            turn_on_fast_radiator_ventilation(client, topic_living_room_radiator)
        else:
            client = mqtt.Client()
            client.connect(broker_ip, broker_port)
            turn_on_slow_radiator_ventilation(client, topic_living_room_radiator)
    elif current_temp > target_temp:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_off(client, topic_circulation)
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_off_radiator_ventilation(client, topic_living_room_radiator)
