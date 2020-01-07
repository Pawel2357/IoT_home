import os
import time
import datetime
import numpy as np
import slack

client = slack.WebClient(token='xyz')
climate_data_file = '/home/pawel/Documents/IoT_home/data/climate_data.csv'
soc_data_file = '/home/pawel/Documents/IoT_home/data/soc_data.csv'
climate_data_stairs_filename = '/home/pawel/Documents/IoT_home/data/stairs_climate_data.csv'

temp_min_v = 19.5
temp_max_v = 22
soc_low_v = 41
soc_high_v = 90
humid_low_v = 25
humid_high_v = 60
soc_6_off_v = 40
soc_6_on_v = 46
condensation_s_humid_v = 62
condensation_s_temp_v = 13

class Notification:
    def __init__(self, notification_delay, channel='#smart-home'):
        self.last_notification_time = None
        self.notification_delay = notification_delay
        self.channel = channel
    
    def can_notify(self):
        if self.last_notification_time is not None:
            now = datetime.datetime.now()
            d_t = now - self.last_notification_time
            print(d_t.seconds / 60)
            if d_t.seconds / 60 > self.notification_delay:
                self.last_notification_time = datetime.datetime.now()
                return True
            else:
                return False
        else:
            self.last_notification_time = datetime.datetime.now()
            return True
        
    def reset(self):
        self.last_notification_time = None
        
    def send_message(self, message):
        response = client.chat_postMessage(
            channel=self.channel,
            text=message)
        assert response["ok"]
        assert response["message"]["text"] == message
            
    def notify(self, message):
        self.send_message(message)
    
def read_temperature(data_file):
    with open(data_file) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        humidity = []

        for row in last10:
            x = row.split(",")
            temp.append(float(x[2]))
            humidity.append(float(x[1][1:]))
    temp = np.mean(temp)
    humidity = np.mean(humidity)
    return temp, humidity

def read_soc(filename_soc):
    with open(filename_soc) as csv_file:
        list = csv_file.readlines()[-10:]
        soc_l = []
        for row in list:
            soc = row.split(",")
            soc_l.append(int(soc[1]))
    return np.mean(soc_l)

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

low_temp = Notification(60)
high_temp = Notification(60)
SoC_high = Notification(60)
SoC_low = Notification(60)
humid_low = Notification(60)
humid_high = Notification(60)
condensation_stairs_warning = Notification(60)
cable_6_on = Notification(360)
cable_6_off = Notification(360)

while True:
    soc = read_soc(soc_data_file)
    temp, humidity = read_temperature(climate_data_file)
    temp_s, humidity_s = read_temp_humid_stairs(climate_data_stairs_filename)
    time.sleep(20)
    if soc < soc_low_v and SoC_low.can_notify():
        SoC_low.notify("Low battery charge: " + str(soc))
        SoC_high.reset()
    if soc > soc_high_v and SoC_high.can_notify():
        SoC_high.notify("High battery charge: " + str(soc))
        SoC_low.reset()
    if soc < soc_6_off_v and cable_6_off.can_notify():
        cable_6_off.notify("Wire 6 battery off: " + str(soc))
        cable_6_on.reset()
    if soc > soc_6_on_v and cable_6_on.can_notify():
        cable_6_on.notify("Wire 6 battery on: " + str(soc))
        cable_6_off.reset()
    if temp < temp_min_v and low_temp.can_notify():
        low_temp.notify("Low temperature: " + str(temp))
        high_temp.reset()
    if temp > temp_max_v and high_temp.can_notify():
        high_temp.notify("High temperature: " + str(temp))
        low_temp.reset()
    if humidity < humid_low_v and humid_low.can_notify():
        humid_low.notify("Low humidity: " + str(humidity))
        humid_high.reset()
    if humidity > humid_high_v and humid_high.can_notify():
        humid_high.notify("High humidity: " + str(humidity))
        humid_low.reset()
    if humidity_s > condensation_s_humid_v and condensation_stairs_warning.can_notify():
        condensation_stairs_warning.notify("Wall condensation, humid: " + str(humidity_s))
    if condensation_s_temp_v < temp_s and humidity_s > condensation_s_humid_v - 5 and condensation_stairs_warning.can_notify():
        condensation_stairs_warning.notify("Wall condensation, humid: " + str(humidity_s) + " temp: " + str(temp_s))
