# Interface to Pololu SSC03A servo controller using the Pololu protocol.
# Based on code from:
# http://dmt195.wordpress.com/2009/01/19/python-to-interface-with-the-pololu-8-channel-servo-controller/

import serial
import sys
#set up the serial port for action
ser=serial.Serial('/dev/ttyUSB0')
ser.baudrate=9600
def setspeed(n,speed):
  #Quick check that things are in range
  if speed > 127 or speed <0:
    speed=1
    print "WARNING: Speed should be between 0 and 127. Setting speed to 1..."
    print "Setting servo "+str(n)+" speed to "+str(speed)+" out of 127."
  speed=int(speed)
  #set speed (needs 0x80 as first byte, 0x01 as the second, 0x01 is for speed, 0 for servo 0, and 127 for max speed)
  bud=chr(0x80)+chr(0x01)+chr(0x01)+chr(n)+chr(speed)
  ser.write(bud)

def setpos(n,angle):
  #Check that things are in range
  if angle > 180 or angle <0:
    angle=90
    print "WARNING: Angle range should be between 0 and 180. Setting angle to 90 degrees to be safe..."
    print "moving servo "+str(n)+" to "+str(angle)+" degrees."

  #Valid range is 500-5500
  offyougo=int(5000*angle/180)+500
  #Get the lowest 7 bits
  byteone=offyougo&127
  #Get the highest 7 bits
  bytetwo=(offyougo-(offyougo&127))/128
  #move to an absolute position in 8-bit mode (0x04 for the mode, 0 for the servo, 0-255 for the position (spread over two bytes))
  bud=chr(0x80)+chr(0x01)+chr(0x04)+chr(n)+chr(bytetwo)+chr(byteone)
  ser.write(bud)

mode=sys.argv[1]
n=int(sys.argv[2])
m=int(sys.argv[3])
if mode=='speed':
  setspeed(n,m)
elif mode=='pos':
  setpos(n,m)
else:
  print "No commands given.\nUsage: servo_ctl_pololu.py speed <servo> <speed>, or\n servo_ctl_pololu.py pos <servo> <angle>"
