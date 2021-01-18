import sys
sys.path.append("/home/pawel/Documents/IoT_home/IoT_home")
import config as cfg
import datetime
import BlynkLib
import time
import numpy as np
import _thread
import threading
import paho.mqtt.client as mqtt
from sklearn.linear_model import LinearRegression


broker_ip = cfg.broker_ip
broker_port = cfg.broker_port

global thr
global thr_b
global is_listenning_mqtt
is_listenning_mqtt = False

BLYNK_AUTH = 'exZj6wubF6aoiNofhE2MbE9aoNCgbE5L'
blynk = BlynkLib.Blynk(BLYNK_AUTH,
                       server='xyz',        # set server address
                       port=8080,                       # set server port
                       heartbeat=30,                    # set heartbeat to 30 secs
                       #log=print                       # use print function for debug logging
                       )


climate_data_file = "/home/pawel/Documents/IoT_home/data/climate_kitchen_data.csv"
bathroom_climate_data_file = "/home/pawel/Documents/IoT_home/data/climate_bathroom_data.csv"

def control_kitchen_ventilation(turn_off):
    temp, humidity = get_temp_humidity(climate_data_file)
    reg = LinearRegression().fit(np.array(range(len(humidity))).reshape(-1, 1), humidity)
    print(reg.coef_[0])
    time.sleep(5)
    if reg.coef_[0] > 0.07 and reg.coef_[0] < 0.2:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("kitchen_fan", "2")
        time.sleep(30)
        return True
    elif reg.coef_[0] > 0.2 and reg.coef_[0] < 0.4:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("kitchen_fan", "4")
        time.sleep(70)
        return True
    elif reg.coef_[0] > 0.4:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("kitchen_fan", "7")
        time.sleep(60)
        return True
    elif turn_off:
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("kitchen_fan", "0")
        return False

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
            turn_off = True
            turn_off = control_kitchen_ventilation(turn_off)
            print("still running")
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("kitchen_fan", "0")
        print('turn off')
        print("stopped!")
        
global turn_off_count
        
class StoppableThreadBath(threading.Thread):
    "Thread class with a stop() method. The thread itself has to check regularly for the stopped() condition."
    def __init__(self):
        super(StoppableThreadBath, self).__init__()
        self._stop_event = threading.Event()
        
    def stop(self):
        self._stop_event.set()

    def join(self, *args, **kwargs):
        self.stop()
        super(StoppableThreadBath,self).join(*args, **kwargs)

    def run(self):
        global turn_off_count
        print("start running")
        is_listenning_mqtt = False
        turn_off_count = 0
        
        while not self._stop_event.is_set():
            time.sleep(30)
            localtime = datetime.datetime.now()
            temp_bathroom, humidity_bathroom = get_temp_humidity(bathroom_climate_data_file)
            print('temp_bathroom', temp_bathroom)
            print('humidity_bathroom', humidity_bathroom)
                
            if np.mean(humidity_bathroom) > 82.0:
                client = mqtt.Client()
                client.connect(broker_ip, broker_port)
                turn_off_count = 0
                if np.mean(humidity_bathroom) > 90:
                    client.publish("bathroom_fan", "5")
                else:
                    client.publish("bathroom_fan", "4")
            else:
                vent_min = 5
                vent_speed = "4"
                if localtime.minute < vent_min:
                    print("run bathroom fan")
                    client = mqtt.Client()
                    client.connect(broker_ip, broker_port)
                    client.publish("bathroom_fan", vent_speed)
                    turn_off_count = 0
                else:
                    if turn_off_count < 3:
                        turn_off_count += 1
                        client = mqtt.Client()
                        client.connect(broker_ip, broker_port)
                        client.publish("bathroom_fan", "0")  
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("bathroom_fan", "0")
        print('turn off')
        print("stopped!")

def control_ventilator(client, topic, speed):
    client.publish(topic, speed)

# Register Virtual Pins
@blynk.VIRTUAL_WRITE(4)
def my_write_handler(value):
    client = mqtt.Client()
    client.connect(broker_ip, 1883)
    print(value)
    if value[0] == '1':
        control_ventilator(client, "kitchen_fan", "0")
    elif value[0] == '2':
        print('slow')
        control_ventilator(client, "kitchen_fan", "2")
    elif value[0] == '3':
        print('fast')
        control_ventilator(client, "kitchen_fan", "4")
    elif value[0] == '4':
        control_ventilator(client, "kitchen_fan", "7")
    elif value[0] == '5':
        control_ventilator(client, "kitchen_fan", "9")
        
# Register Virtual Pins
@blynk.VIRTUAL_WRITE(5)
def my_write_handler(value):
    client = mqtt.Client()
    client.connect(broker_ip, 1883)
    print(value)
    if value[0] == '1':
        control_ventilator(client, "bathroom_fan", "0")
    elif value[0] == '2':
        print('slow')
        control_ventilator(client, "bathroom_fan", "2")
    elif value[0] == '3':
        print('fast')
        control_ventilator(client, "bathroom_fan", "4")
    elif value[0] == '4':
        control_ventilator(client, "bathroom_fan", "7")
    elif value[0] == '5':
        control_ventilator(client, "bathroom_fan", "9")
        
        
# Control kitchen ventilation
@blynk.VIRTUAL_WRITE(6)
def my_write_handler(value):
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
        client.publish("kitchen_fan", "0")
    elif value[0] == '2': # charge in loop 1h15m and 45m break
        try:
            thr.join()
            print('join thread')
        except:
            pass
        thr = StoppableThread()
        thr.start()
        
# Control kitchen ventilation
@blynk.VIRTUAL_WRITE(7)
def my_write_handler(value):
    global thr_b
    print(value)
    if value[0] == '1':
        try:
            thr_b.join()
            print('join thread')
        except:
            pass
        client = mqtt.Client()
        client.connect(broker_ip, broker_port)
        client.publish("bathroom_fan", "0")
    elif value[0] == '2': # charge in loop 1h15m and 45m break
        try:
            thr_b.join()
            print('join thread')
        except:
            pass
        thr_b = StoppableThreadBath()
        thr_b.start()

def get_temp_humidity(climate_data_file):
    with open(climate_data_file) as csvfile:
        last10 = list(csvfile)[-10:]
        temp = []
        humidity = []
        for row in last10:
            x = row.split(",")
            try:
                temp.append(float(x[2][:-2]))
                humidity.append(float(x[1][1:]))
            except:
                pass
    return temp, humidity

# Register Virtual Pins
while(True):
    blynk.run()
    time.sleep(1)
    temp, humidity = get_temp_humidity(climate_data_file)
    temp_bathroom, humidity_bathroom = get_temp_humidity(bathroom_climate_data_file)
    try:
        blynk.virtual_write(0, '{:.2f}'.format(np.mean(temp)))
        blynk.virtual_write(1, '{:.2f}'.format(np.mean(humidity)))
        blynk.virtual_write(2, '{:.2f}'.format(np.mean(temp_bathroom)))
        blynk.virtual_write(3, '{:.2f}'.format(np.mean(humidity_bathroom)))
    except:
        pass
