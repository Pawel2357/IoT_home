import paho.mqtt.client as mqtt
import time
import datetime
import numpy as np


# This is the Subscriber

broker_ip = "xyz"
topic = "climate_stairs"
path_temp_data = '/home/pawel/Documents/IoT_home/data/stairs_climate_data.csv'

humidity_thr = 62

def read_temp_humid_stairs(data_filename):
    with open(data_filename) as csvfile:
        last10 = list(csvfile)[-100:]
    temp = []
    humidity = []
    for row in last10:
        x = row.split(",")
        temp.append(float(x[2][:-2]))
        humidity.append(float(x[1][1:]))

    humidity_correct = []
    temp_correct = []
    for i, h in enumerate(humidity):
        if h <= 100:
            humidity_correct.append(h)
            temp_correct.append(temp[i])
    return np.mean(temp_correct[-3:]), np.mean(humidity_correct[-3:])

def turn_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic, "0")
    time.sleep(2)

def turn_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic, "4")
    time.sleep(2)
    
def log_temp():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic, "t")
    time.sleep(2)
    

# morning wall condensation
while True:
    time.sleep(30)
    log_temp()
    time.sleep(30)
    t, h = read_temp_humid_stairs(path_temp_data)
    if h > humidity_thr:
        turn_on()
    elif t > 13.2 and h > humidity_thr - 5:
        turn_on()
    else:
        turn_off()
