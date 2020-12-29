## The aim of this project is to create car safety assistant that can predict potential dengerous road situations and trigger notifications and alarms.
### Raspberry pi objects classification
This project is based on the article https://www.pyimagesearch.com/2018/02/19/real-time-object-detection-on-the-raspberry-pi-with-the-movidius-ncs/

#### Run script on startup in raspberry pi:
Add command to run script in file `sudo vim /etc/rc.local`

#### To run objects classifier run
`python3 real_2.py --graph graphs/mobilenetgraph --display 0`

#### How to run objects classifier script on startup:
1. Make sure the following command is working:
`sudo /usr/bin/python3 /home/pi/realtime-object-detection/real_2.py --graph /home/pi/realtime-object-detection/graphs/mobilenetgraph --display 0`
2. Open crontab using `sudo crontab -e`
3. Add line `@reboot sleep 60 && /usr/bin/python3 /home/pi/realtime-object-detection/real_2.py --graph /home/pi/realtime-object-detection/graphs/mobilenetgraph --display 0 >> /var/log/real_2.log 2>&1`

### Arduino waterproof distance sensor
- https://www.makerguides.com/jsn-sr04t-arduino-tutorial/

### Line detection
- https://towardsdatascience.com/line-detection-make-an-autonomous-car-see-road-lines-e3ed984952c

### 6 dof arduino tutorial
- https://github.com/pololu/lsm6-arduino

### GPS arduino
- https://lastminuteengineers.com/neo6m-gps-arduino-tutorial/
