Systemd files to start the camera software automatically. Based on the
tutorial by Nuno Alves: http://www.nunoalves.com/open_source/?p=308

Assuming that the vidsrv and audiosrc applications are located in
/opt/bonecam/bin/

Copy bonecam-app.sh to /opt/bonecam/bin/
Copy bonecam-app.service to /lib/systemd/system/

cd /etc/systemd/system/
ln -s /lib/systemd/system/bonecam-app.service bonecam-app.service

Finally, activate the service and enable the service to be started on boot-up:
systemctl daemon-reload
systemctl start  bonecam-app.service
systemctl enable bonecam-app.service

