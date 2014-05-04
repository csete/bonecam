Systemd files to start the camera software automatically. Based on the
tutorial by Nuno Alves: http://www.nunoalves.com/open_source/?p=308

Assuming that the vidsrv application is located in /opt/bonecam/bin/

Copy autostart-video.sh to /opt/bonecam/bin/
Copy bonecam-video.service to /lib/systemd/system/

cd /etc/systemd/system/
ln -s /lib/systemd/system/bonecam-video.service bonecam-video.service

Finally, activate the service and enable the service to be started on boot-up:
systemctl daemon-reload
systemctl start bonecam-video.service
systemctl enable bonecam-video.service
