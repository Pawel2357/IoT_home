## Create static IP on server

Ubuntu https://linuxhint.com/change-from-dhcp-to-static-ip-address-ubuntu/  
Router using device mac adress https://www.tp-link.com/pl/support/faq/714/

## Run mosquito
`sudo apt-get update`  
`sudo apt-get install mosquitto`  
`sudo apt-get install mosquitto-clients`

### Test mosquitto  
`mosquitto_sub -t "test"`  
`mosquitto_pub -m "message from mosquitto_pub client" -t "test"`

## Run web app
### Install flask
`pip3 install flask`  
`tmux`  
`python3 app_name.py &`  
`tmux detach`

## To-Do next steps
Improve the server by using gunicorn. See https://blog.miguelgrinberg.com/post/the-flask-mega-tutorial-part-xvii-deployment-on-linux-even-on-the-raspberry-pi
