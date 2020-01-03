import paho.mqtt.client as mqtt
import time
import datetime
import numpy as np


# This is the Subscriber

broker_ip = "xyz"
kitchen_floor_heating_topic = "kitchen_floor_heating"
path_temp_data = '/home/pawel/Documents/IoT_home/data/kitchen_floor_temp_data.csv'

start_time_m = datetime.time(hour=4, minute=0)
end_time_m = datetime.time(hour=8, minute=0)

start_time_e = datetime.time(hour=15, minute=0)
end_time_e = datetime.time(hour=22, minute=0)
floor_temp = 28
samples = 1

def turn_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(kitchen_floor_heating_topic, "0")
    time.sleep(2)

def turn_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(kitchen_floor_heating_topic, "4")
    time.sleep(2)
    
def log_temp():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(kitchen_floor_heating_topic, "t")
    time.sleep(2)
    
def get_temperature(filename):
    with open(filename) as csvfile:
        last10 = list(csvfile)[-samples:]
        temp = []
        for row in last10:
            x = row.split(",")
            temp.append(float(x[1]))
    temp = np.mean(temp)
    return temp

    
# morning heating
while True:
    time.sleep(30)
    log_temp()
    time.sleep(30)
    temp = get_temperature(path_temp_data)
    now = datetime.datetime.now()
    now_d = datetime.time(hour=now.hour, minute=now.minute)
    if now_d < end_time_m and now_d > start_time_m and temp < floor_temp:
        turn_on()
    elif now_d < end_time_e and now_d > start_time_e and temp < floor_temp:
        turn_on()
    else:
        turn_off()
