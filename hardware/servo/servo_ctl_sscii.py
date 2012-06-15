# Interface to Pololu SSC03A servo controller running in Mini SSC II Mode.
# Based on code from:
# http://dmt195.wordpress.com/2009/01/19/python-to-interface-with-the-pololu-8-channel-servo-controller/

import serial
import sys
#set up the serial port for action (0==COM1==ttyS0)
ser=serial.Serial('/dev/ttyUSB0')
ser.baudrate=9600

def setpos(n,angle):
  #Quick check that things are in range
  if angle > 180 or angle <0:
    angle=90
    print "WARNING: Angle range should be between 0 and 180. Setting angle to 90 degrees to be safe..."
    print "moving servo "+str(n)+" to "+str(angle)+" degrees."

  byteone=int(254*angle/180)
  #move to an absolute position in 8-bit mode (0x04 for the mode, 0 for the servo, 0-255 for the position (spread over two bytes))
  bud=chr(0xFF)+chr(n)+chr(byteone)
  ser.write(bud)

mode=sys.argv[1]
n=int(sys.argv[2])
m=int(sys.argv[3])

if mode=='pos':
  setpos(n,m)
else:
  print "No commands given.\nUsage: servo_ctl_sscii.py pos <servo> <angle>"

