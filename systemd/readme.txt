Systemd files to start the camera software automatically. 

Assuming that the vidsrv and audiosrv applications are located in
/opt/bonecam/bin/

Copy bonecam-app.sh to /opt/bonecam/bin/
Copy bonecam-app.service to /lib/systemd/system/

Reload the systemd service database:

  systemctl daemon-reload

You can now use:
systemctl start   bonecam-app.service
systemctl stop    bonecam-app.service
systemctl enable  bonecam-app.service
systemctl disable bonecam-app.service

Enabling the service wil lensure that it is started automatically at boot.
Disabling it will prevent the service to be started at boot.
