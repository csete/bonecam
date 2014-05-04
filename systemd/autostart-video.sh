#!/bin/bash

mkdir -p /opt/bonecam/log
NOW="$(date --utc +%Y%m%d_%H%M%S)"
LOGFILE="/opt/bonecam/log/vidsrv_$NOW.log"

# start video server
/opt/bonecam/bin/vidsrv -a 192.168.1.100 > $LOGFILE 2>&1 &

# start video
#exec /home/root/bin/video-server 192.168.1.100 4000 > /dev/null 2>&1
