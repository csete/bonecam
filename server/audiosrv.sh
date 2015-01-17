#!/bin/bash
#
# Streams 44.1 kHz mono, vorbis encoded audio.
# CPU load ~ 50%.
# CPU load could be reduced by using the native sample rate of the card.
#
# Otherwise one can use raw audio through TCP connection which uses low CPU
# but high bitrate.
#
# NB: Check the audio device number before running the script!
#

usage ()
{
    echo "Usage:  audio-server <host_ip> <host_port>"
}


if [ $# -ne 2 ]; then
    usage
    exit 0
fi

echo "Starting audio streaming server to $1:$2"

. /home/debian/setup_env.sh

## Vorbis encoded audio
gst-launch-1.0 -v -e alsasrc device="plughw:1" ! audio/x-raw,format=\(string\)S16LE,rate=44100,channels=1 ! audioconvert ! vorbisenc ! rtpvorbispay ! udpsink host=$1 port=$2


## Raw audio via tcp
#while true; do
#    gst-launch-1.0 -v -e alsasrc device="plughw:1" ! audio/x-raw,format=\(string\)S16LE,rate=48000,channels=1 ! tcpclientsink host=$1 port=$2 sync=true
#    sleep 5
#done

