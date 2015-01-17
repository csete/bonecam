#!/bin/bash

DEST=192.168.1.1
VPORT=40000
APORT=41000

# start video server
/opt/bonecam/bin/videosrv.sh $DEST $VPORT  &
# NB: Remove the trailing "&" above if the audio is disabled

# start audio
exec /opt/bonecam/bin/audiosrv.sh $DEST $APORT

