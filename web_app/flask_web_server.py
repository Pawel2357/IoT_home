from flask import Flask, render_template, request
import paho.mqtt.client as mqtt
import time


def change_color(client, r, g, b, intensity):
    if r != '':
        client.publish("lamp_1", "r" + str(r))
    time.sleep(0.05)
    if g != '':
        client.publish("lamp_1", "g" + str(g))
    time.sleep(0.05)
    if b != '':
        client.publish("lamp_1", "b" + str(b))
    time.sleep(0.05)
    if intensity != '':
        client.publish("lamp_1", "i" + str(intensity))

app = Flask(__name__)


@app.route("/", methods=['GET', 'POST'])
def index():
    print(request.method)
    if request.method == 'POST':
        if request.form.get('Turn on') == 'Turn on':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            client.publish("sensors_3d_printing_room", "0")
            time.sleep(1)
        elif  request.form.get('Turn off') == 'Turn off':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            client.publish("sensors_3d_printing_room", "1")
            time.sleep(1)
        elif request.form.get('beast on') == 'beast on':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            client.publish("control_1", "0")
            time.sleep(1)
        elif request.form.get('beast off') == 'beast off':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            client.publish("control_1", "1")
            time.sleep(1)
        elif request.form.get('change color') == 'change color':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            r = request.form['r']
            g = request.form['g']
            b = request.form['b']
            intensity = request.form['intensity']
            change_color(client, r, g, b, intensity)
        elif request.form.get('Off lamp') == 'Off lamp':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 255, 255, 255, 0)
        elif request.form.get('White') == 'White':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 255, 255, 255, 255)
        elif request.form.get('Blue') == 'Blue':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 0,191,255, 255)
        elif request.form.get('Yellow') == 'Yellow':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 255,255,0, 255)
        elif request.form.get('Green') == 'Green':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 50,205,50, 255)
        elif request.form.get('Red') == 'Red':
            client = mqtt.Client()
            client.connect("40.121.36.126", 1883)
            change_color(client, 255,69,0, 255)
        else:
            # pass # unknown
            with open("/home/pawel2357/3d_printer.txt") as f:
                data = f.readlines()
            lastline = data[-1]
            with open("/home/pawel2357/gps_data_correct.txt") as f:
                data = f.readlines()
            last_car = data[-1]
            return render_template("index.html", value=lastline[-150:], car_value=last_car[-150:])
    elif request.method == 'GET':
        # return render_template("index.html")
        print("No Post Back Call")
    with open("/home/pawel2357/3d_printer.txt") as f:
        data = f.readlines()
    lastline = data[-1]
    with open("/home/pawel2357/gps_data_correct.txt") as f:
        data = f.readlines()
    last_car = data[-1]
    return render_template("index.html", value=lastline[-150:], car_value=last_car[-150:])
    
if __name__ == '__main__':
    app.run(host = '0.0.0.0')
