# Image for bonecam

require ti-hw-bringup-image.bb

ROOTFSTYPE_beaglebone = "ext4"

IMAGE_INSTALL += " \
	systemd-analyze \
	task-sdk-target \
	vim vim-vimrc \
	procps \
	beaglebone-tester \
	screen minicom \
	git \
	led-config \
	opencv-dev \
	cronie-systemd ntpdate \
	nano \
	minicom \
	hicolor-icon-theme \
	gateone \
	tar \
	gdb gdbserver \
	mplayer2 \
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

