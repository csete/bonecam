#
# Host side audio receiver for raw TCP stream.
#

gst-launch-1.0 -e -v tcpserversrc host=0.0.0.0 port=4100 ! audio/x-raw,format=\(string\)S16LE,rate=48000,channels=1 ! autoaudiosink

