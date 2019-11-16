from flask import Flask, render_template, request
import paho.mqtt.client as mqtt
import time
import numpy as np

broker_IP = "xxx"
bathroom_topic = "lamp_1"


def change_color(client, r, g, b, intensity, topic):
    if r != '':
        client.publish(topic, "r" + str(r))
    time.sleep(0.05)
    if g != '':
        client.publish(topic, "g" + str(g))
    time.sleep(0.05)
    if b != '':
        client.publish(topic, "b" + str(b))
    time.sleep(0.05)
    if intensity != '':
        client.publish(topic, "i" + str(intensity))

app = Flask(__name__)


@app.route("/", methods=['GET', 'POST'])
def index():
    print(request.method)
    if request.method == 'POST':
        if request.form.get('Turn on') == 'Turn on':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("sensors_3d_printing_room", "0")
            time.sleep(1)
        elif  request.form.get('Turn off') == 'Turn off':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("sensors_3d_printing_room", "1")
            time.sleep(1)
        elif request.form.get('beast on') == 'beast on':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("control_1", "0")
            time.sleep(1)
        elif request.form.get('beast off') == 'beast off':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("control_1", "1")
            time.sleep(1)
        elif request.form.get('Living room renovable on') == 'Living room renovable on':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("renovable_control", "6")
            time.sleep(1)
        elif request.form.get('Living room renovable off') == 'Living room renovable off':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            client.publish("renovable_control", "e")
            time.sleep(1)
        elif request.form.get('change color') == 'change color':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            r = request.form['r']
            g = request.form['g']
            b = request.form['b']
            intensity = request.form['intensity']
            change_color(client, r, g, b, intensity, bathroom_topic)
        elif request.form.get('Off lamp') == 'Off lamp':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 255, 255, 255, 0, bathroom_topic)
        elif request.form.get('White') == 'White':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 255, 255, 255, 255, bathroom_topic)
        elif request.form.get('Blue') == 'Blue':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 0, 191, 255, 255, bathroom_topic)
        elif request.form.get('Yellow') == 'Yellow':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 255, 255, 0, 255, bathroom_topic)
        elif request.form.get('Green') == 'Green':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 50, 205, 50, 255, bathroom_topic)
        elif request.form.get('Red') == 'Red':
            client = mqtt.Client()
            client.connect(broker_IP, 1883)
            change_color(client, 255, 69, 0, 255, bathroom_topic)
        else:
            with open('/home/pi/climate_data.csv') as csvfile:
                last10 = list(csvfile)[-10:]
                temp = []
                humidity = []
                print(last10)
                for row in last10:
                    x = row.split(",")
                    temp.append(float(x[2]))
                    humidity.append(float(x[1][1:]))
            temp = np.mean(temp)
            humidity = np.mean(humidity)
            lastline = "Temperature =" + str(temp) + " Humidity =" + str(humidity)
#             with open("/home/pawel2357/gps_data_correct.txt") as f:
#                 data = f.readlines()
            last_car = "Cooming soon :)"
            return render_template("index.html", value=lastline, car_value=last_car)
    elif request.method == 'GET':
        # return render_template("index.html")
        print("No Post Back Call")
#     with open("/home/pawel2357/3d_printer.txt") as f:
#         data = f.readlines()
    lastline = "Cooming soon :)"
#     with open("/home/pawel2357/gps_data_correct.txt") as f:
#         data = f.readlines()
    last_car = "Cooming soon :)"
    return render_template("index.html", value=lastline, car_value=last_car)
    
if __name__ == '__main__':
    app.run(host = '0.0.0.0', debug=True)
