import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import csv
import BlynkLib
import time
import datetime
from datetime import date
import numpy as np
import pandas as pd
import _thread
import threading
import paho.mqtt.client as mqtt
import config as cfg

SAVE_LOGS_FILE = '/home/pawel/Documents/IoT_home/data/temp_logs.csv'
set_type_dict = {'1': 'turn off', '2': 'Auto set', '3': 'Set', '4': 'On', '5': 'Auto set T'}

global thr
global temp_set
temp_set = 20

def find_today_nb_rows(filename):
    df = pd.read_csv(filename)
    df.columns = ['date', 'rest']
    df.tail()
    df_cor = df.tail(14000)
    df_cor['datetime'] = pd.to_datetime(df_cor['date'])
    dt = date.today()
    start_date = datetime.datetime.combine(dt, datetime.datetime.min.time())
    df_cor['datetime'] 
    mask = df_cor['datetime'] > start_date
    nb_rows = len(df_cor.loc[mask])
    return nb_rows

def get_consumption(filename='/home/pawel/Documents/IoT_home/data/power_7_data.csv'):
    with open(filename) as csvfile:
        nb_rows = find_today_nb_rows(filename)
        last10 = list(csvfile)[-nb_rows:]
        power_7 = []
        power_hp = []
        power_kitchen = []
        battery_out = []
        for row in last10:
            x = row.split(",")
            try:
                if float(x[7]) > 300:
                    power_hp.append(float(x[7]))
            except:
                pass
    print('len(power_hp)', len(power_hp))
    if len(power_hp) > 0:
        energy_hp = np.mean(power_hp) * len(power_hp) * 0.1 / (60 * 1000)
        return energy_hp
    else:
        return 0

def log_temp_set(value_0, target_temp, save_filename=SAVE_LOGS_FILE):
    with open(SAVE_LOGS_FILE, mode='a') as logs_file:
        logs_writer = csv.writer(logs_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        localtime = time.asctime(time.localtime(time.time()))
        logs_writer.writerow([localtime, '{:.2f}'.format(target_temp), set_type_dict[value_0]])

def turn_circulation_on(client, topic):
    print("circulation on")
    client.publish(topic, "1")

def turn_circulation_off(client, topic):
    print("circulation off")
    client.publish(topic, "0")
    
def turn_off_radiator_ventilation(client, topic):
    print("off ventilation")
    client.publish(topic, "0")
    
def turn_on_slow_radiator_ventilation(client, topic):
    print("slow ventilation")
    client.publish(topic, "6")

def turn_on_medium_slow_radiator_ventilation(client, topic):
    print("medioum slow ventilation")
    client.publish(topic, "7")
    
def turn_on_medium_radiator_ventilation(client, topic):
    print("medium ventilation")
    client.publish(topic, "8")

def turn_on_fast_radiator_ventilation(client, topic):
    client.publish(topic, "9")
    
def get_temp(climate_data_file):
    with open(climate_data_file) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        for row in last10:
            x = row.split(",")
            temp.append(float(x[2]))
    temp = np.mean(temp)
    return temp

def get_target_temperature(day_temp=21.0):
    now = datetime.datetime.now()
    hour = now.hour
    if hour > 3 and hour <= 6:
        blynk.virtual_write(6, '{:.1f}'.format(19.4))
        log_temp_set('2', 19.4)
        return 19.4, 0
    elif hour > 6 and hour <= 20:
        blynk.virtual_write(6, '{:.1f}'.format(day_temp))
        log_temp_set('2', day_temp)
        return day_temp, 2
    elif (hour > 20 and hour <= 23) or (hour >= 0 and hour <= 3):
        blynk.virtual_write(6, '{:.1f}'.format(18.0))
        log_temp_set('2', 18.0)
        return 18.0, 6
    else:
        blynk.virtual_write(6, '{:.1f}'.format(20.0))
        log_temp_set('2', 20.0)
        return 20.0, 4

def control_temperature_standard(topic_circulation="circulation_pump",  topic_living_room_radiator="radiator_living_room", climate_data_file='/home/pawel/Documents/IoT_home/data/climate_data.csv', is_constant_temp=False, constant_temp=21, is_turbo=False, day_temp=21.0): 
    time.sleep(30)
    current_temp = get_temp(climate_data_file)
    if not is_constant_temp:
        target_temp, period_type = get_target_temperature(day_temp=day_temp)
        if is_turbo:
            period_type = 0
    else:
        print("constant temp mode, temp:", constant_temp)
        target_temp = constant_temp
        if is_turbo:
            period_type = 0
        else:
            period_type = 1
    print("current_temp", current_temp)
    print("target_temp", target_temp)
    print("period_type", period_type)
    delta = 0.1
    if current_temp < target_temp:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_on(client, topic_circulation)
        if period_type == 0:
            client = mqtt.Client()
            client.connect(broker_ip, broker_port)
            turn_on_fast_radiator_ventilation(client, topic_living_room_radiator)
        else:
            if target_temp - current_temp > 1.3:
                client = mqtt.Client()
                client.connect(broker_ip, broker_port)
                turn_on_medium_radiator_ventilation(client, topic_living_room_radiator)
            elif target_temp - current_temp > 0.5:
                turn_on_medium_slow_radiator_ventilation(client, topic_living_room_radiator)
            else:
                turn_on_slow_radiator_ventilation(client, topic_living_room_radiator)
    elif current_temp > target_temp + delta:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_off(client, topic_circulation)
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_off_radiator_ventilation(client, topic_living_room_radiator)


BLYNK_AUTH = 'XvFwE7tPAxGp-lQ6P0Lhh7p2iOpYV8ut'
blynk = BlynkLib.Blynk(BLYNK_AUTH,
                       server='xyz',        # set server address
                       port=8080,                       # set server port
                       heartbeat=30,                    # set heartbeat to 30 secs
                       #log=print                       # use print function for debug logging
                       )

broker_ip = cfg.broker_ip
broker_port = cfg.broker_port
topic = cfg.topic_charger
charging_time = cfg.charging_time

climate_data_file = '/home/pawel/Documents/IoT_home/data/climate_data.csv'
topic_circulation = "circulation_pump"
topic_living_room_radiator = "radiator_living_room"

def get_temp_humidity(climate_data_file):
    with open(climate_data_file) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        humidity = []
        air_quality = []
        for row in last10:
            x = row.split(",")
            temp.append(float(x[2]))
            if float(x[1][1:]) < 100:
                humidity.append(float(x[1][1:]))
            air_quality.append(float(x[3][:-2]))
    temp = np.mean(temp)
    if len(humidity) > 0:
        humidity = np.mean(humidity)
    else:
        humidity = 0
    air_quality = np.mean(air_quality)
    return temp, humidity, air_quality


class StoppableThread(threading.Thread):
    "Thread class with a stop() method. The thread itself has to check regularly for the stopped() condition."

    def __init__(self, is_constant_temp=False, constant_temp=21, is_turbo=False, day_temp=21.0):
        super(StoppableThread, self).__init__()
        self._stop_event = threading.Event()
        self.is_constant_temp = is_constant_temp
        self.constant_temp = constant_temp
        self.is_turbo = is_turbo
        self.day_temp = day_temp

    def stop(self):
        self._stop_event.set()

    def join(self, *args, **kwargs):
        self.stop()
        super(StoppableThread,self).join(*args, **kwargs)

    def run(self):
        print("start running")
        while not self._stop_event.is_set():
            control_temperature_standard(is_constant_temp=self.is_constant_temp, constant_temp=self.constant_temp, is_turbo=self.is_turbo, day_temp=self.day_temp)
            print("still running")
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_off(client, topic_circulation)
        turn_off_radiator_ventilation(client, topic_living_room_radiator)
        print('turn off')
        print("stopped!")
        
        
@blynk.VIRTUAL_WRITE(3)
def my_write_handler(value):
    global temp_set
    log_temp_set(value[0], temp_set)
    global thr
    print(value)
    if value[0] == '1':
        try:
            thr.join()
            print('join thread')
        except:
            pass
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_off(client, topic_circulation)
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_off_radiator_ventilation(client, topic_living_room_radiator)
        print('turn off')
    elif value[0] == '2': # const auto temp
        try:
            blynk.virtual_write(6, '{:.1f}'.format(temp_set))
            thr.join()
            print('join thread')
        except:
            pass
        thr = StoppableThread(day_temp=temp_set)
        thr.start()
    elif value[0] == '3': # const set temp
        try:
            blynk.virtual_write(6, '{:.1f}'.format(temp_set))
            thr.join()
            print('join thread')
        except:
            pass
        thr = StoppableThread(is_constant_temp=True, constant_temp=temp_set)
        thr.start()
    elif value[0] == '4': # turn on
        try:
            thr.join()
            print('join thread')
        except:
            pass
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_circulation_on(client, topic_circulation)
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        turn_on_slow_radiator_ventilation(client, topic_living_room_radiator)
    elif value[0] == '5': # Auto set with turbo
        try:
            blynk.virtual_write(6, '{:.1f}'.format(temp_set))
            thr.join()
            print('join thread')
        except:
            pass
        thr = StoppableThread(day_temp=temp_set, is_turbo=True)
        thr.start()
        
@blynk.VIRTUAL_WRITE(5)
def my_write_handler(value):
    print(value)
    global temp_set
    temp_set = float(value[0])
    print(temp_set)
        
def log_temp():
    print("log temp works")
    while(True):
        time.sleep(1)
        temp, humidity, air_quality = get_temp_humidity(climate_data_file)
        blynk.virtual_write(0, '{:.2f}'.format(temp))
        blynk.virtual_write(1, '{:.2f}'.format(humidity))
        blynk.virtual_write(2, '{:.2f}'.format(air_quality))
        
def display_temp():
    print("display temp works")
    while(True):
        time.sleep(20)
        temp, humidity, air_quality = get_temp_humidity(climate_data_file)
        blynk.virtual_write(0, '{:.2f}'.format(temp))
        # energy_hp = get_consumption()
        # blynk.virtual_write(7, '{:.2f}'.format(energy_hp * 0.73))
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        print("publish")
        client.publish("Touch_screen_temp", '{:.2f}'.format(temp))

_thread.start_new_thread(log_temp,() )
_thread.start_new_thread(display_temp,() )

while True:
    blynk.run()
