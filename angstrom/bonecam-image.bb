# Image for bonecam

require ti-hw-bringup-image.bb

ROOTFSTYPE_beaglebone = "ext4"

IMAGE_INSTALL += " \
	systemd-analyze \
	task-sdk-target \
	vim vim-vimrc \
	procps \
	screen \
	minicom \
	git \
	led-config \
	opencv-dev \
	cronie-systemd ntpdate \
	nano \
	gateone \
	tar \
	gdb gdbserver \
	tslib-tests tslib-calibrate \
	iproute2 canutils \
	connman-tests \
	rsync \
	media-ctl yavta \
	v4l-utils \
	gstreamer \
	gst-plugins-base \
	gst-plugins-good \
	gst-plugins-bad \
"

export IMAGE_BASENAME = "Bonecam"

