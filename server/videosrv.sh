#!/bin/bash
#
# Streams H.264 encoded video iver RTP/UDP.
# CPU load on BBB < 10%.

usage ()
{
    echo "Usage:  vidsrv <dest_ip> <dest_port>"
}


if [ $# -ne 2 ]; then
    usage
    exit 0
fi

echo "Starting video streaming server to $1:$2"

# Logitech C920
/opt/bonecam/bin/gst-launch-1.0 -v -e \
    uvch264src initial-bitrate=3000000 average-bitrate=3000000 \
    iframe-period=5000 device=/dev/video0 name=src auto-start=true src.vidsrc ! \
    video/x-h264,width=1280,height=720,framerate=30/1 ! h264parse ! \
    rtph264pay ! udpsink host=$1 port=$2

# Haupauge HD-PVR
#cat /dev/video0 | /opt/bonecam/bin/gst-launch-1.0 -v -e fdsrc fd=0 ! \
#    tsparse ! tsdemux ! h264parse ! rtph264pay ! \
#    tee name=t1 ! queue ! udpsink host=192.168.10.50 port=9056 \
#    t1. ! queue ! udpsink host=192.168.10.52 port=9056

