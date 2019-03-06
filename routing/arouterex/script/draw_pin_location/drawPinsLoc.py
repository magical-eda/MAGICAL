#!/usr/bin/python3
import sys
import matplotlib.pyplot as plt
class PinData():
    def __init__(self):
        self.netIdx = -1
        self.xCord = 0
        self.yCord = 0
        self.layer = -1

class location():
    def __init__(self):
        self.xCord = 0
        self.yCord = 0
        self.zCord = 0

    def __eq__(self, another):
        return self.xCord == another.xCord and self.yCord == another.yCord and self.zCord == another.zCord
    
    def __hash__(self):
        return hash(self.xCord) ^ hash(self.yCord) ^ hash(self.zCord)

def parseanoynmousePinData(anoynmouse):
    splited = [x.strip() for x in anoynmouse.split(',')]
    pin = PinData()
    pin.netIdx = splited[0]
    pin.xCord = splited[1]
    pin.yCord = splited[2]
    pin.layer = splited[3]
    return pin

def readPinLocation():
    csvFile = str(sys.argv[1])

    pinArray = []
    with open(csvFile) as fin:
        anoynmouseIdx = 0
        for anoynmouse in fin:
            if (anoynmouseIdx == 0):
                anoynmouseIdx  += 1
                continue
            pin = parseanoynmousePinData(anoynmouse)
            pinArray.append(pin)
            anoynmouseIdx += 1

    return pinArray

def drawPinLocation(figIdx, pinArray):
    plt.figure(figIdx)
    for pin in pinArray:
        plt.scatter(pin.xCord, pin.yCord)
    plt.show()
    figIdx += 1

#main
figIdx = 0
pinArray = readPinLocation()
drawPinLocation(figIdx)
