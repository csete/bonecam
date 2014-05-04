#!/bin/bash

# initialize camera ho H.264
/home/root/bin/cam init > /dev/null 2>&1

# start audio
/home/root/bin/audio-server 192.168.1.100 4002 > /dev/null 2>&1 &

# start video
exec /home/root/bin/video-server 192.168.1.100 4000 > /dev/null 2>&1
