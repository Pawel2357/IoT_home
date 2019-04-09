## How to copy files from local computer to server using scp:
1. Generate ssh-key on local computer `ssh-keygen -t rsa`
2. Add rsa.pub to server using `ssh-copy-id -i ~/.ssh/id_rsa.pub USER@SERVER`
3. Now `ssh` or `scp` can be used without authentication.

### Add crontab command to upload image every 5 min
`*/5 * * * * fswebcam /home/pi/xy.jpg && scp /home/pi/xy.jpg xy@40.121.36.126:/xy/abc/static/xy.jpg`
