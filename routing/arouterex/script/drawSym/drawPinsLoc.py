#!/usr/bin/python3
import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from PIL import Image
import numpy as np
class PinData():
    def __init__(self):
        self.xLo = 100000000000
        self.yLo = 100000000000
        self.xHi = -10000000000
        self.yHi = - 10000000000


def parseanoynmousePinData(anoynmouse):
    splited = anoynmouse.split()
    pin = PinData()
    pin.layer = splited[0]
    itsx =True
    for num in range(1, len(splited)):
        if itsx == True:
            itsx = False
            if (pin.xLo > int(splited[num])):
                pin.xLo = int(splited[num])
            if (pin.xHi < int(splited[num])):
                pin.xHi = int(splited[num])
        else:
            itsx = True
            if (pin.yLo > int(splited[num])):
                pin.yLo = int(splited[num])
            if (pin.yHi < int(splited[num])):
                pin.yHi = int(splited[num])

    return pin

def readPinLocation(idx):
    csvFile = str(sys.argv[idx])

    pinArray = []
    with open(csvFile) as fin:
        anoynmouseIdx = 0
        for anoynmouse in fin:
            pin = parseanoynmousePinData(anoynmouse)
            pinArray.append(pin)
            anoynmouseIdx += 1

    return pinArray

def drawPinLocation(figIdx, pinArray1, pinArray2):
#calculate the needed xlim and ylim
    xlimLo = 10000000000000
    ylimLo = 10000000000000
    xlimHi = -10000000000000
    ylimHi = -10000000000000

    for pin in pinArray1:
        if pin.xLo < xlimLo:
            xlimLo = pin.xLo
        if pin.xHi > xlimHi:
            xlimHi = pin.xHi
        if pin.yLo < ylimLo:
            ylimLo = pin.yLo
        if pin.yHi > ylimHi:
            ylimHi = pin.yHi
    for pin in pinArray2:
        if pin.xLo < xlimLo:
            xlimLo = pin.xLo
        if pin.xHi > xlimHi:
            xlimHi = pin.xHi
        if pin.yLo < ylimLo:
            ylimLo = pin.yLo
        if pin.yHi > ylimHi:
            ylimHi = pin.yHi

    xlimLo = xlimLo - xlimLo * 0.1
    ylimLo = ylimLo - ylimLo * 0.1
    xlimHi = xlimHi + xlimHi * 0.1
    ylimHi = ylimHi + ylimHi * 0.1

#Add two nets pins
    
    fig = plt.figure(figIdx)
    ax1 = fig.add_subplot(111, aspect='equal')
    for pin in pinArray1:
        ax1.add_patch(
                patches.Rectangle((pin.xLo, pin.yLo), pin.xHi - pin.xLo, pin.yHi - pin.yLo, color='red'))
    plt.xlim(xlimLo, xlimHi)
    plt.ylim(ylimLo, ylimHi)
    ax2 = fig.add_subplot(111, aspect='equal')
    for pin in pinArray2:
        ax2.add_patch(
                patches.Rectangle((pin.xLo, pin.yLo), pin.xHi - pin.xLo, pin.yHi - pin.yLo, color='blue'))
    plt.xlim(xlimLo, xlimHi)
    plt.ylim(ylimLo, ylimHi)
    plt.show()
    figIdx += 1

#main
figIdx = 0
pinArray1 = readPinLocation(1)
pinArray2 = readPinLocation(2)
drawPinLocation(figIdx, pinArray1, pinArray2)
