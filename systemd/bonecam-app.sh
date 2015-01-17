#!/bin/bash

# start video server
/opt/bonecam/bin/vidsrv -a 192.168.254.22 -p 9040  &
# NB: Remove the trailing "&" above if the audio is disabled

# start audio
exec /opt/bonecam/bin/audiosrv 192.168.254.22 9100 > $AUDLOG 2>&1
