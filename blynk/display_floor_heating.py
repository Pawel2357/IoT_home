import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import numpy as np
import config as cfg
import paho.mqtt.client as mqtt
import time
import datetime
import csv
import BlynkLib
import _thread
import threading
global blynk
from sklearn.linear_model import LinearRegression


filename = cfg.floor_temp_data
broker_ip = cfg.broker_ip
broker_port = cfg.broker_port
kitchen_floor_heating_topic = cfg.kitchen_floor_heating_topic
topic_ventilation = "ventilation_living_room"
topic_ventilation_bedroom = "ventilation_bedroom"
charging_topic = "charging_1"
stop_threads = False
outdoor_aq_threshold = 50
global thr

BLYNK_AUTH = 'Bo7ol7uuvgx7Gy4uGHQ0j8eVgC_6Za-r'
blynk = BlynkLib.Blynk(BLYNK_AUTH,
                       server=broker_ip,        # set server address
                       port=8080,                       # set server port
                       heartbeat=30,                    # set heartbeat to 30 secs
                       #log=print                       # use print function for debug logging
                       )

def save_to_csv(csv_file_path, value):
    localtime = time.asctime(time.localtime(time.time()))
    with open(csv_file_path, mode='a') as file_soc:
        soc_data_writer = csv.writer(file_soc, delimiter=',')
        soc_data_writer.writerow([localtime, value])

def get_temperature(filename):
    with open(filename) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        print("last10", last10)
        for row in last10:
            x = row.split(",")
            temp.append(float(x[1]))
        print("temp", temp)
    temp = np.mean(temp)
    return temp

def get_aq_bedroom(filename, size=1):
    with open(filename) as csvfile:
        last10 = list(csvfile)[-size:]
        aq = []
        aq_float = []
        for row in last10:
            x = row.split(",")
            aq.append(float(x[1][1:]))
            aq_float.append(float(x[2][:-2]))
    aq = np.mean(aq)
    aq_float = np.mean(aq_float)
    return aq, aq_float

def log_temp():
    client_3 = mqtt.Client()
    client_3.connect(broker_ip, broker_port)
    client_3.publish(kitchen_floor_heating_topic, "t")
    print("log temperature")
    time.sleep(2)


def off_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "4")
    time.sleep(0.05)
    client.publish(topic, "5")

def slow_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "0")
    time.sleep(0.05)
    client.publish(topic, "5")

def fast_ventilation(client, topic):
    time.sleep(0.05)
    client.publish(topic, "0")
    time.sleep(0.05)
    client.publish(topic, "1")
    

def get_outdoor_temp_humidity(climate_data_file='/home/pawel/Documents/IoT_home/data/outdoor_climate_data.csv'):
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
    def __init__(self):
        super(StoppableThread, self).__init__()
        self._stop_event = threading.Event()
        
    def stop(self):
        self._stop_event.set()

    def join(self, *args, **kwargs):
        self.stop()
        super(StoppableThread,self).join(*args, **kwargs)

    def run(self):
        print("start running")
        while not self._stop_event.is_set():
            localtime = datetime.datetime.now()
            temp, humidity, air_quality = get_outdoor_temp_humidity()
            time.sleep(10)
            if localtime.hour >= 3 and localtime.hour <= 23:
                if humidity < 90:
                    if air_quality < 20:
                        vent_min = min(30, 110 - humidity)
                    else:
                        vent_min = 15
                else:
                    if air_quality < 20:
                        vent_min = min(20, 110 - humidity)
                    else:
                        vent_min = 13
                print('localtime.minute', localtime.minute)
                print('vent_min', vent_min)
                if localtime.minute < vent_min:
                    client = mqtt.Client()
                    client.connect(broker_ip, broker_port)
                    print("slow vent")
                    slow_ventilation(client, topic_ventilation)
                else:
                    client = mqtt.Client()
                    client.connect(broker_ip, broker_port)
                    print("off vent")
                    off_ventilation(client, topic_ventilation)
            else:
                client = mqtt.Client()
                client.connect(broker_ip, broker_port)
                off_ventilation(client, topic_ventilation)
            print("still running")
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        off_ventilation(client, topic_ventilation)
        print('turn off')
        print("stopped!")

    
# Register Virtual Pins
@blynk.VIRTUAL_WRITE(0)
def my_write_handler(value):
    global thr
    client = mqtt.Client()
    client.connect(broker_ip, 1883)
    print(value)
    if value[0] == '1':
        try:
            thr.join()
            print('join thread')
        except:
            pass
        print('off')
        off_ventilation(client, topic_ventilation)
    elif value[0] == '2':
        try:
            thr.join()
            print('join thread')
        except:
            pass
        print('slow')
        slow_ventilation(client, topic_ventilation)
    elif value[0] == '3':
        try:
            thr.join()
            print('join thread')
        except:
            pass
        print('fast')
        fast_ventilation(client, topic_ventilation)
    elif value[0] == '4':
        try:
            thr.join()
            print('join thread')
        except:
            pass
        thr = StoppableThread()
        thr.start()
        
def is_air_clean(measure_time=260, fast=False):
    client = mqtt.Client()
    client.connect(broker_ip, 1883)
    aq, aq_float = get_aq_bedroom('/home/pawel/Documents/IoT_home/data/aq_bedroom.csv', size=140)
    if fast:
        off_ventilation(client, topic_ventilation_bedroom)
    else:
        slow_ventilation(client, topic_ventilation_bedroom)
    time.sleep(measure_time)
    aq_final, aq_float_final = get_aq_bedroom('/home/pawel/Documents/IoT_home/data/aq_bedroom.csv', size=140)
    return aq_float_final, aq_float

def get_regression_coefficient(X, y):
    reg = LinearRegression().fit(X, y)
    return reg.coef_

def get_measure_time(run_iterations, p_coeff=1.03, c_coeff=5, max_run_iterations=7):
    run_iterations = run_iterations % max_run_iterations
    return 260 + (run_iterations * c_coeff) * p_coeff**run_iterations
    
def get_sleep_time(run_iterations, p_coeff=1.03, c_coeff=5, max_run_iterations=6):
    run_iterations = run_iterations % max_run_iterations
    return (45 + (run_iterations * c_coeff)* p_coeff**run_iterations) * 60
    
        
def auto_ventilation(fast=True):
    aq_lst = []
    run_iterations = 0
    while True:
        if stop_threads:
            break
        else:
            if run_iterations >= 3:
                aq_float_final, aq_float = is_air_clean(measure_time=get_measure_time(run_iterations), fast=fast)
            else:
                aq_float_final, aq_float = is_air_clean(measure_time=get_measure_time(run_iterations))
            if len(aq_lst) < 1:
                aq_lst.append(aq_float)
                aq_lst.append(aq_float_final)
            else:
                aq_lst.append(aq_float_final)
                if len(aq_lst) > 4:
                    aq_lst = aq_lst[-4:]
                
            coef = get_regression_coefficient(np.array(aq_lst).reshape(-1, 1), np.array(range(len(aq_lst))).reshape(-1, 1))
            print("aq_lst", aq_lst)
            print("coef", coef)
            save_to_csv('/home/pawel/Documents/IoT_home/data/aq_bedroom_average_260_140.csv', str(aq_lst[-1]) + ',' + str(coef[0][0]))
            
            if (coef[0][0] > 0 and np.mean(aq_lst[-2:]) > 130) or (coef[0][0] > 0.05 and np.mean(aq_lst[-2:]) <= 130):
                # turn off ventilation
                client = mqtt.Client()
                client.connect(broker_ip, 1883)
                fast_ventilation(client, topic_ventilation_bedroom) # meaning depends on connection
                time.sleep(get_sleep_time(run_iterations))
                aq_lst = []
                run_iterations = 0
        run_iterations += 1

def auto_ventilation_night(fast=False):
    aq_lst = []
    was_turned_off = False
    run_iterations = 0
    while True:
        now = datetime.datetime.now()
        if stop_threads_n:
            break
        else:
            if now.hour > 22 or now.hour < 7:
                was_turned_off = False
                if len(aq_lst) > 3 and coef[0][0] < - 1:
                    aq_float_final, aq_float = is_air_clean(fast=fast)
                else:
                    aq_float_final, aq_float = is_air_clean(measure_time=get_measure_time(run_iterations, c_coeff=8))
                if len(aq_lst) < 1:
                    aq_lst.append(aq_float)
                    aq_lst.append(aq_float_final)
                else:
                    aq_lst.append(aq_float_final)
                    if len(aq_lst) > 4:
                        aq_lst = aq_lst[-4:]
                run_iterations += 1
                coef = get_regression_coefficient(np.array(aq_lst).reshape(-1, 1), np.array(range(len(aq_lst))).reshape(-1, 1))
                print("aq_lst", aq_lst)
                print("coef", coef)
                save_to_csv('/home/pawel/Documents/IoT_home/data/aq_bedroom_average_260_140_night.csv', str(aq_lst[-1]) + ',' + str(coef[0][0]))
                if (coef[0][0] > 0 and np.mean(aq_lst[-2:]) > 130) or (coef[0][0] > 0.05 and np.mean(aq_lst[-2:]) <= 130):
                    # turn off ventilation
                    client = mqtt.Client()
                    client.connect(broker_ip, 1883)
                    fast_ventilation(client, topic_ventilation_bedroom) # meaning depends on connection
                    time.sleep(get_sleep_time(run_iterations))
                    aq_lst = []
                    run_iterations = 0
            else:
                # turn off ventilation
                if not was_turned_off:
                    client = mqtt.Client()
                    client.connect(broker_ip, 1883)
                    fast_ventilation(client, topic_ventilation_bedroom) # meaning depends on connection
                    was_turned_off = True
                run_iterations = 0
                
        
# Register Virtual Pins
@blynk.VIRTUAL_WRITE(5)
def my_write_handler(value):
    client = mqtt.Client()
    client.connect(broker_ip, 1883)
    print(value)
    if value[0] == '3':
        print('off')
        stop_threads = True
        stop_threads_n = True
        off_ventilation(client, topic_ventilation_bedroom)
    elif value[0] == '2':
        print('slow')
        stop_threads = True
        stop_threads_n = True
        slow_ventilation(client, topic_ventilation_bedroom)
    elif value[0] == '1':
        print('fast')
        stop_threads = True
        stop_threads_n = True
        fast_ventilation(client, topic_ventilation_bedroom)
    elif value[0] == '4':
        global stop_threads
        stop_threads = False
        thr = threading.Thread(target=auto_ventilation)
        print('start ventilation')
        thr.start()  
    elif value[0] == '5':
        global stop_threads_n
        stop_threads_n = False
        thr = threading.Thread(target=auto_ventilation_night)
        print('start ventilation night')
        thr.start()  
        
def turn_on_heating():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, broker_port)
    client_2.publish(kitchen_floor_heating_topic, "4")
    time.sleep(2)
    
def turn_off_heating():
    client_2 = mqtt.Client()
    client_2.connect(broker_ip, broker_port)
    client_2.publish(kitchen_floor_heating_topic, "0")
    time.sleep(2)
        
        
def control_floor_heating(target_temp, stop):
    while True:
        if stop(): 
            break
        temp = get_temperature(filename)
        print('heating temp', temp)
        if temp < target_temp:
            turn_on_heating()
        else:
            turn_off_heating()
        time.sleep(2)

                
#t1 = threading.Thread(target=control_floor_heating, args=(t, lambda: stop_threads))
@blynk.VIRTUAL_WRITE(4)
def my_write_handler(value):
    print(value)
    global stop_threads
    if value[0] == '1':
        stop_threads = False
        #thr = threading.Thread(target=test_thread, args=(20, ))
        #print('start heating')
        #thr.start()
    elif value[0] == '0':
        stop_threads = True
        print("turn off")
    
    
def read_temperature():
    global blynk
    while True:
        time.sleep(15)
        log_temp()
        temp = get_temperature(filename)
        print("temp floor", temp)
        blynk.virtual_write(1, '{:.2f}'.format(temp))
        
def read_aq_bedroom():
    global blynk
    while True:
        time.sleep(5)
        aq, aq_float = get_aq_bedroom('/home/pawel/Documents/IoT_home/data/aq_bedroom.csv')
        blynk.virtual_write(6, '{:.2f}'.format(aq))
        blynk.virtual_write(7, '{:.2f}'.format(aq_float))
        
        
def get_outdoor_air_quality(filename):
    with open(filename) as csvfile:
        last10 = list(csvfile)[-10:]
        air_q = []
        for row in last10:
            x = row.split(",")
            air_q.append(float(x[3][:-2]))
    air_q = np.mean(air_q)
    return air_q

        
def check_air_quality():
    global blynk
    filename = "/home/pawel/Documents/IoT_home/data/outdoor_climate_data.csv"
    while True:
        time.sleep(15)
        aq = get_outdoor_air_quality(filename)
        if aq > outdoor_aq_threshold:
            client = mqtt.Client()
            client.connect(broker_ip, 1883)
            off_ventilation(client, topic_ventilation)
            off_ventilation(client, topic_ventilation)
            client = mqtt.Client()
            client.connect(broker_ip, 1883)
            fast_ventilation(client, topic_ventilation_bedroom)
            fast_ventilation(client, topic_ventilation_bedroom)
        
        
def log_temperature():
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code " + str(rc))
        topic = cfg.kitchen_floor_temp_log_topic
        client.subscribe(topic)


    def on_message(client, userdata, msg):
        localtime = time.asctime(time.localtime(time.time()))
        data = str(msg.payload.decode("utf-8"))
        print(data)
        with open(filename, mode='a') as file_soc:
            soc_data_writer = csv.writer(file_soc, delimiter=',')
            soc_data_writer.writerow([localtime, data])

    client = mqtt.Client()
    client.connect(broker_ip, broker_port)

    client.on_connect = on_connect
    client.on_message = on_message

    client.loop_forever()
    
    
def log_aq_bedroom():
    filename = '/home/pawel/Documents/IoT_home/data/aq_bedroom.csv'
    def on_connect(client, userdata, flags, rc):
        print("Connected with result code " + str(rc))
        topic = "ventilation_bedroom_aq_log"
        client.subscribe(topic)

    def on_message(client, userdata, msg):
        localtime = time.asctime(time.localtime(time.time()))
        data = str(msg.payload.decode("utf-8"))
        print(data)
        with open(filename, mode='a') as file_soc:
            soc_data_writer = csv.writer(file_soc, delimiter=',')
            soc_data_writer.writerow([localtime, data])

    client = mqtt.Client()
    client.connect(broker_ip, broker_port)

    client.on_connect = on_connect
    client.on_message = on_message

    client.loop_forever()
    
        
_thread.start_new_thread(read_temperature,() )
_thread.start_new_thread(log_temperature,() )
_thread.start_new_thread(log_aq_bedroom,() )
_thread.start_new_thread(read_aq_bedroom,() )
# _thread.start_new_thread(check_air_quality(),() )

global blynk
while True:
    blynk.run()
  
