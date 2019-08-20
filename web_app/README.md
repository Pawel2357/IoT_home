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
