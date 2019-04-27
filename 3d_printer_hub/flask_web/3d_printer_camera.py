import paho.mqtt.client as mqtt
import os

# This is the Subscriber

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("3d_printer_camera_rpi_3")

def on_message(client, userdata, msg):
    if msg.payload.decode("utf-8") == "take_photo":
        os.system("fswebcam /home/pi/photo_3d_printer_state.jpg")
        os.system("scp /home/pi/photo_3d_printer_state.jpg pawel2357@40.121.36.126:/home/pawel2357/photo_3d_printer_state_rp_3.jpg")


client = mqtt.Client()
client.connect("40.121.36.126", 1883)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()