## Set up ssh on raspberry to work correctly for a long time
https://www.raspberrypi.org/forums/viewtopic.php?f=28&t=138631&start=100

## Smart garden sensor to measure plant growth. It upload photos of garden every day at specific time. The photos are uploaded to Twitter.

### Install dependencies
1. `sudo apt-get install fswebcam`
2. `sudo apt-get install -y python3-oauth python3-oauth2client python3-oauthlib python3-requests-oauthlib`
3. `sudo pip3 install twython==3.6.0`

### Set up crontab to run on boot
1. Open `sudo crontab -e`
2. Add line `sleep 90 && sudo /usr/bin/python3 /home/pi/twitter_post_sensors/post_to_twitter.py`

