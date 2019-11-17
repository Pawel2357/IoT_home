import paho.mqtt.client as mqtt
import time
import csv
import datetime

broker_ip = "xyz"
topic_ventilation = "ventilation_living_room"


def turn_on():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic_ventilation, "1")
    time.sleep(2)


def turn_off():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, 1883)
    client_2.publish(topic_ventilation, "0")
    time.sleep(2)

while True:
    with open('/home/pi/climate_data.csv') as csvfile:
        last10 = list(csvfile)[-10:]
        humidity = []
        for row in last10:
            x = row.split(",")
            humidity.append(float(x[1][1:]))
    humidity = np.mean(humidity)
    now = datetime.datetime.now()
    time.sleep(25)
    if now.hour % 2 == 0 and now.minute <= 25 and humidity < 50:
        turn_on()
        time.sleep(10)
    else:
        turn_off()
        time.sleep(10)
