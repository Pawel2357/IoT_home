import os
import time
import datetime
import numpy as np
import slack

client = slack.WebClient(token='xoxp-837260216032-824471975746-839473999942-2f3f9c7be1d876e8844cec9a79994bd6')
climate_data_file = '/home/pawel/Documents/IoT_home/data/climate_data.csv'
soc_data_file = '/home/pawel/Documents/IoT_home/data/soc_data.csv'

low_temp = False
high_temp = False
SoC_high = False
SoC_low = False
humid_low = False
humid_high = False
cable_6_on = False
cable_6_off = False


temp_min_v = 15
temp_max_v = 22
soc_low_v = 41
soc_high_v = 90
humid_low_v = 25
humid_high_v = 60
soc_6_off_v = 40
soc_6_on_v = 46


def send_message(message, channel='#smart-home'):
    response = client.chat_postMessage(
        channel=channel,
        text=message)
    assert response["ok"]
    assert response["message"]["text"] == message
    
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

def logic():
    return True, False

while True:
    soc = read_soc(soc_data_file)
    temp, humidity = read_temperature(climate_data_file)
    time.sleep(20)
    now = datetime.datetime.now()
    if now.minute == 0:
        low_temp = False
        high_temp = False
        SoC_high = False
        SoC_low = False
        humid_low = False
        humid_high = False
    if soc < soc_low_v and not SoC_low:
        SoC_low, SoC_high = logic()
        send_message("Low battery charge: " + str(soc))
    if soc > soc_high_v and not SoC_high:
        SoC_high, SoC_low = logic()
        send_message("High battery charge: " + str(soc))
    if soc < soc_6_off_v and not cable_6_off:
        cable_6_off, cable_6_on = logic()
        send_message("Wire 6 battery off: " + str(soc))
    if soc > soc_6_on_v and not cable_6_on:
        cable_6_on, cable_6_off = logic()
        send_message("Wire 6 battery on: " + str(soc))
    if temp < temp_min_v and not low_temp:
        low_temp, high_temp = logic()
        send_message("Low temperature: " + str(temp))
    if temp > temp_max_v and not high_temp:
        high_temp, low_temp = logic()
        send_message("High temperature: " + str(temp))
    if humidity < humid_low_v and not humid_low:
        humid_low, humid_high = logic()
        send_message("Low humidity: " + str(humidity))
    if humidity > humid_high_v and not humid_high:
        humid_high, humid_low = logic()
        send_message("High humidity: " + str(humidity))
