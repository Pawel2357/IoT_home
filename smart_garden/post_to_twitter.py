from twython import Twython
import os

from auth import (
    consumer_key,
    consumer_secret,
    access_token,
    access_token_secret
)

twitter = Twython(
    consumer_key,
    consumer_secret,
    access_token,
    access_token_secret
)

message = "Hello world!"
# twitter.update_status(status=message)
print("Tweeted: %s" % message)

os.system('fswebcam /home/pi/twitter_post_sensors/garden_1.jpg')
image = open('/home/pi/twitter_post_sensors/garden_1.jpg', 'rb')

response = twitter.upload_media(media=image)
media_id = [response['media_id']]
twitter.update_status(status=message, media_ids=media_id)
os.system('sudo halt -p')
