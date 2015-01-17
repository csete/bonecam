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
    echo "Usage:  audiosrv <host_ip> <host_port>"
}


if [ $# -ne 2 ]; then
    usage
    exit 0
fi

echo "Starting audio streaming server to $1:$2"

## Opus encoded audio (C920 does 1 channel 32 kHz)
/opt/bonecam/bin/gst-launch-1.0 -v -e alsasrc device="plughw:1" ! \
    audio/x-raw, format=\(string\)S16LE, rate=32000, channels=1 ! \
    audioresample ! \
    audio/x-raw, format=\(string\)S16LE, rate=48000, channels=1 ! \
    opusenc bandwidth=1103 bitrate=32000 frame-size=40 complexity=3 ! \
    rtpopuspay ! udpsink host=$1 port=$2

## Vorbis encoded audio
#/opt/bonecam/bin/gst-launch-1.0 -v -e alsasrc device="plughw:1" ! \
#    audio/x-raw,format=\(string\)S16LE,rate=44100,channels=1 ! \
#    audioconvert ! vorbisenc ! rtpvorbispay ! udpsink host=$1 port=$2

