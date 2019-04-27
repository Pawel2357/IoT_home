from flask import Flask, render_template, request, url_for
import paho.mqtt.client as mqtt
import os
import time

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
printer_photos = os.path.join('static')
full_filename = "/aa/2357/xy.jpg"
app.config['upload_folder'] = printer_photos
@app.route("/")
def hello():
    client = mqtt.Client()
    client.connect("40.121.36.126", 1883)
    client.publish("3d_printer_camera_rpi_3", "take_photo")
    time.delay(3)
    full_filename = os.path.join(app.config['upload_folder'], '/home/pawel2357/photo_3d_printer_state_rp_3.jpg')
    return render_template("index.html", user_image = full_filename)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=5001)
