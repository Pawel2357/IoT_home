import os
import time
import numpy as np
import slack

client = slack.WebClient(token='iii')
climate_data_file = 'path'
soc_data_file = 'path'

low_temp = False
high_temp = False
SoC_high = False
SoC_low = False
humid_low = False
humid_high = False

temp_min_v = 15
temp_max_v = 22
soc_low_v = 35
soc_high_v = 90
humid_low_v = 25
humid_high_v = 60

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
    time.sleep(5)
    if soc < soc_low_v and not SoC_low:
        SoC_low, SoC_high = logic()
        send_message("Low battery charge: " + str(soc))
    if soc > soc_high_v and not SoC_high:
        SoC_high, SoC_low = logic()
        send_message("High battery charge: " + str(soc))
    if temp < temp_min_v and not low_temp:
        low_temp, high_temp = logic()
        send_message("Low temperature: " + str(soc))
    if temp > soc_high_v and not high_temp:
        high_temp, low_temp = logic()
        send_message("High temperature: " + str(soc))
    if humidity < humid_low_v and not humid_low:
        humid_low, humid_high = logic()
        send_message("Low humidity: " + str(humidity))
    if humidity > humid_high_v and not humid_high:
        humid_high, humid_low = logic()
        send_message("High humidity: " + str(humidity))
