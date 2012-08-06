#!/bin/bash
/home/root/bin/cam init > /dev/null 2>&1
exec /home/root/bin/video-server 192.168.254.141 4000 > /dev/null 2>&1

