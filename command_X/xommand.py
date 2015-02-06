__author__ = 'khjtony_M4800'
'''
    only pick up first joystick
    Please press "start" to calibrate
    the frame is:

    [button] [sign] [value] \n

    #0xff is start delim
    [button] refers to which button/axis is triggered
    [sign] refers to the sign of value, or, because our value is unsigned value
    [value] is value scaled to 0x00 to 0xFE
'''

import pygame
import serial
from KSerialUtil import XBeeConnector
import time
import math
import numpy as np


# Define some colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
pygame.init()

# Define Xbee
# XBee = XBeeConnector.XBeeConnector()
# XBee.init()
XBee_port = raw_input("Input Xbee port: ")
def XBee_write(msg):
    ser = serial.Serial(XBee_port.upper(), 19200, timeout=1)
    ser.bytesize = serial.EIGHTBITS
    ser.parity = serial.PARITY_NONE
    ser.stopbits = serial.STOPBITS_ONE
    ser.write(msg)
    ser.close()

# Define calibration (only for Joy 1 and Joy 2) for 360 controller
axis_cali = np.zeros(5)
button_cali = np.zeros(8)
hat_cali = np.zeros(2)
cali_flag = 0


# Define command array to be sent
commandArr = list()

# Set the width and height of the screen [width,height]
# size = [500, 700]
# screen = pygame.display.set_mode(size)

# pygame.display.set_caption("My Game")

# Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joysticks
pygame.joystick.init()

# LOL for joy and serial
joystick_index = input("Please input # of joystick. (Default: 0)")


# -------- Main Program Loop -----------
while not done:
    pygame.event.get()
    # EVENT PROCESSING STEP
    # for event in pygame.event.get():  # User did something
    #     if event.type == pygame.QUIT:  # If user clicked close
    #         done = True  # Flag that we are done so we exit this loop
    #
    #     # Possible joystick actions: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN
    #     # JOYBUTTONUP JOYHATMOTION
    #     if event.type == pygame.JOYBUTTONDOWN:
    #         print("Joystick button pressed.")
    #     if event.type == pygame.JOYBUTTONUP:
    #         print("Joystick button released.")


    # DRAWING STEP
    # First, clear the screen to white. Don't put other drawing commands
    # above this, or they will be erased with this command.
    # screen.fill(WHITE)
    # textPrint.reset()

    # Get count of joysticks
    # joystick_count = pygame.joystick.get_count()

    # print "Number of joysticks: {}".format(joystick_count)
    # textPrint.indent()

    # For each joystick:
    # for i in range(joystick_count):
    joystick = pygame.joystick.Joystick(joystick_index)
    joystick.init()  # print "Joystick {}".format(i)
    # textPrint.indent()

    # Get the name from the OS for the controller/joystick
    # name = joystick.get_name()
    # print "Joystick name: {}".format(name)

    # Usually axis run in pairs, up/down for one, and left/right for
    # the other.
    axes = joystick.get_numaxes()
    # print "Number of axes: {}".format(axes)
    # textPrint.indent()
    for i in range(axes):
        axis = joystick.get_axis(i)
        commandArr.append([chr(i), chr(1 if axis >0 else 0), chr(int(math.fabs(axis-axis_cali[i])*0xfe))])
        # print "Axis {} value: {:>6.3f}".format(i, axis)
    # textPrint.unindent()

    buttons = joystick.get_numbuttons()

    # turn on calibration
    if joystick.get_button(0) is 1:
        cali_flag = 1
    else:
        cali_flag = 0

    # print "Number of buttons: {}".format(buttons)
    # textPrint.indent()

    for i in range(buttons):
        button = joystick.get_button(i)
        commandArr.append([chr(0x10+i), chr(1 if button > 0 else 0), chr(button)])
        # print "Button {:>2} value: {}".format(i, button)
    # textPrint.unindent()

    # Hat switch. All or nothing for direction, not like joysticks.
    # Value comes back in an array.
    hats = joystick.get_numhats()
    # print "Number of hats: {}".format(hats)
    # textPrint.indent()

    for i in range(hats):
        hat = joystick.get_hat(i)
        # print "Hat {} value: {}:{}".format(i, hat[0],hat[1])
        # print type(hat)
        # commandArr.append(0x20+i, int(hat[0])+1, hat[1]+1)
        # textPrint.unindent()

        # textPrint.unindent()


    #send data
    for item in commandArr:
        # item.insert(0xff,0)
        item.append('\n')
        # XBee_write(''.join(item))
        # print ''.join(str(a) for a in item)
        print item

    commandArr = list()
    time.sleep(1)


# ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT

# Go ahead and update the screen with what we've drawn.
# pygame.display.flip()

# Limit to 60 frames per second
clock.tick(60)  # Close the window and quit.
# If you forget this line, the program will 'hang'
# on exit if running from IDLE.
pygame.quit()