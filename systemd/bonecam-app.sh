#!/bin/bash

# start video server
/opt/bonecam/bin/vidsrv.sh dest_ip dest_port  &
# NB: Remove the trailing "&" above if the audio is disabled

# start audio
exec /opt/bonecam/bin/audiosrv.sh dest_ip dest_port

