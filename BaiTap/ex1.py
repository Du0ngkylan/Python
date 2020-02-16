#!/usr/bin/python
 
import time
import os

while 1:
# now = time.strftime("%c")
## date and time representation
    print ("Clock: " + time.strftime("%c"))
    time.sleep(0.2)
    os.system('cls')
 
## Only date representation
# print ("Current date "  + time.strftime("%x"))
 
## Only time representation
# print ("Current time " + time.strftime("%X"))
 
## Display current date and time from now variable 
# print ("Current time %s"  % now )