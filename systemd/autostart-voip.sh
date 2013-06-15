#!/bin/bash

# start server
/home/root/bin/voip-server 192.168.1.100 3002 > /dev/null 2>&1 &

# start client
exec /home/root/bin/voip-client 3000 > /dev/null 2>&1
