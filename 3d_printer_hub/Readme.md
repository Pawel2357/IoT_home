## How to copy files from local computer to server using scp:
1. Generate ssh-key on local computer `ssh-keygen -t rsa`
2. Add rsa.pub to server using `ssh-copy-id -i ~/.ssh/id_rsa.pub USER@SERVER`
3. Now `ssh` or `scp` can be used without authentication.
