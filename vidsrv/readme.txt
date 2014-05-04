Vidsrv is a simple Gstreamer 1.2 application to capture H.264 vidoe from a
Logitech C920 camera and stream it to a network host using RTP/UDP.

The following command line options are available:

  -d <dev>   Camera device. Default is /dev/video0.
  -s <size>  Image size in WxH format e.g. 1280x720 (default).
  -f <num>   Frames per second. Range 5 - 30. Default is 24.
  -b <num>   Bitrate in kilobits / sec. Range 50 - 15000. Default is 3000.
  -i <num>   I-frame period in ms. Range 1000 - 60000. Default is 10000.
  -a <addr>  Hostname or IP address of the mixer. Default is localhost.
  -p <num>   UDP port on the mixer number to stream to. Default is 4000.
  -c <num>   TCP command port number. Default is 4242.
  -h         This help message.

Other settings such as contrast or brightness can be adsjusted using the
v4l2-ctl application.

When the application is running, you can adjust some parameters by sending
simple test commands over TCP to the command port, e.g. by using netcat.
Following commands are savaialble:

  "b 5000"     set bitrate to 5 Mbps
  "i 3000"     set I-frame interval in msec
  "f 30"       set framerate in frames/sec
  "s 640x360"  set frame size

Supported frame sizes: 1920x1080, 1600x896, 1280x720, 1024x576, 960x720,
864x480, 800x600, 800x448 and 640x360.

All resolutions support frame rates: 5, 10, 15, 20, 24, 30.
