IP camera based on Logitech webcam, Beaglebone and gstreamer.

The present v2.1 uses uvch264src for C920 video and works with recent
Beaglebone Debian images and gstreamer 1.4 or later compiled from source
or installed from a Debian testing repository, see for example:
http://www.oz9aec.net/index.php/beaglebone/505-gstreamer-14-on-the-beaglebone.

The gstreamer and systemd scripts assume that gstreamer has been
installed from source using /opt/bonecam/ prefix.


You can still get the old v1.1 image developed in 2012 for the original
Beaglebone running Ångstom by checking out the v1.1 tag.

