#!/bin/bash

usage ()
{
    echo "Usage:  audio-client <port>"
}


if [ $# -ne 1 ]; then
    usage
    exit 0
fi

gst-launch-1.0 -v udpsrc port=$1 ! \
    application/x-rtp, media=audio, clock-rate=48000, payload=96, encoding-name=X-GST-OPUS-DRAFT-SPITTKA-00 ! \
    rtpopusdepay ! opusdec ! alsasink device=plughw:0 sync=false

