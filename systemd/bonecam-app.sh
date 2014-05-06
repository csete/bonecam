#!/bin/bash

mkdir -p /opt/bonecam/log
NOW="$(date --utc +%Y%m%d_%H%M%S)"
VIDLOG="/opt/bonecam/log/vidsrv_$NOW.log"
AUDLOG="/opt/bonecam/log/audiosrv_$NOW.log"

# start video server
/opt/bonecam/bin/vidsrv -a 192.168.254.22 -p 9040 > $VIDLOG 2>&1 &
# NB: Remove the trailing "&" above if the audio is disabled

# start audio
exec /opt/bonecam/bin/audiosrv 192.168.254.22 9100 > $AUDLOG 2>&1

