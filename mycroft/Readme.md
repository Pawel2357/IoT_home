## How to set up mycroft
```git clone https://github.com/MycroftAI/docker-mycroft.git
docker build -t mycroft .

docker run -d -v /home/beast/mycroft/docker-mycroft:/root/.mycroft --device /dev/snd -e PULSE_SERVER=unix:${XDG_RUNTIME_DIR}/pulse/native -v ${XDG_RUNTIME_DIR}/pulse/native:${XDG_RUNTIME_DIR}/pulse/native -v ~/.config/pulse/cookie:/root/.config/pulse/cookie -p 8181:8181 --name mycroft_2 mycroftai/docker-mycroft
```
