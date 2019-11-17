import paho.mqtt.client as mqtt
import time
import datetime
import numpy as np


# This is the Subscriber

broker_ip = "192.168.1.198"
kitchen_floor_heating_topic = "kitchen_floor_heating"

start_time_m = datetime.time(hour=4, minute=35)
end_time_m = datetime.time(hour=6, minute=30)

start_time_e = datetime.time(hour=17, minute=30)
end_time_e = datetime.time(hour=19, minute=00)
min_temp = 22

def turn_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(kitchen_floor_heating_topic, "1")
    time.sleep(2)


def turn_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(kitchen_floor_heating_topic, "0")
    time.sleep(2)

# morning heating
while True:
    time.sleep(20)
    with open('/home/pi/climate_data.csv') as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        for row in last10:
            x = row.split(",")
            temp.append(float(x[2]))
    temp = np.mean(temp)
    now = datetime.datetime.now()
    now_d = datetime.time(hour=now.hour, minute=now.minute)
    if now_d < end_time_m and now_d > start_time_m:
        if temp > min_temp:
            turn_on()
    elif now_d < end_time_e and now_d > start_time_e:
        if temp > min_temp:
            turn_on()
    else:
        turn_off()
