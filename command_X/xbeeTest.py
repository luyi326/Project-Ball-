__author__ = 'khjtony_M4800'

import time
from KSerialUtil import XBeeConnector

XBee = XBeeConnector.XBeeConnector()


def onXBeeMessage(message):
    print message

XBee.on_data_arrive(onXBeeMessage)

XBee.init()
