#!/usr/bin/python3

import sys
import random
import numpy

class Point():
    def __init__(self):
        self.x = 0
        self.y = 0

    def __hash__(self):
        return hash((self.x, self.y))

    def __eq__(self, other):
        return self.x  == other.x and self.y == other.y

class CapAdjust():
    def __init__(self):
        self.xLo = 0
        self.yLo = 0
        self.xHi = 0
        self.yHi = 0
        self.layer = 0
        self.block = 0

class BenchConfig():
    def __init__(self):
        self.tileWidth = 0 
        self.tileHeight = 0
        self.numLayers = 0
        self.direction = [] # 0 for horizontal and 1 for vertical
        self.pitchWidth = []
        self.pitchHeight = []
        self.minWidth = []
        self.minSpacing = []
        self.viaSpacing = []
        self.xNumGrids = 0
        self.yNumGrids = 0
        self.xLo = 0
        self.yLo = 0
        self.capacityRatio = 0.0
        self.capAdjust = []
        self.numNets = 0
        self.numPins = [] # number of pins for each net

def writeDesignSettings(of, benchConfig):
    of.write("grid ")
    of.write(str(benchConfig.xNumGrids))
    of.write( " ")
    of.write(str(benchConfig.yNumGrids))
    of.write(" ")
    of.write(str(benchConfig.numLayers))
    of.write("\nvertical capacity ")
    for layerIdx in range(0, benchConfig.numLayers):
        if (benchConfig.direction[layerIdx] == 0):
            of.write("0 ")
        else:
            of.write(str(max(int(benchConfig.tileWidth * benchConfig.capacityRatio / benchConfig.pitchWidth[layerIdx]), 1)))
            of.write(" ")
    of.write("\nhorizontal capacity ")
    for layerIdx in range(0, benchConfig.numLayers):
        if (benchConfig.direction[layerIdx] == 1):
            of.write("0 ")
        else:
            of.write(str(max(int(benchConfig.tileHeight * benchConfig.capacityRatio/ benchConfig.pitchHeight[layerIdx]), 1)))
            of.write(" ")
    of.write("\nminimum width ")
    for layerIdx in range(0, benchConfig.numLayers):
        of.write(str(benchConfig.minWidth[layerIdx]))
        of.write(" ")
    of.write("\nminimum spacing ")
    for layerIdx in range(0, benchConfig.numLayers):
        of.write(str(benchConfig.minSpacing[layerIdx]))
        of.write(" ")
    of.write("\nvia spacing ")
    for layerIdx in range(0, benchConfig.numLayers):
        of.write(str(benchConfig.viaSpacing[layerIdx]))
        of.write(" ")
    of.write("\n")
    of.write(str(benchConfig.xLo))
    of.write(" ")
    of.write(str(benchConfig.yLo))
    of.write(" ")
    of.write(str(benchConfig.tileWidth))
    of.write(" ")
    of.write(str(benchConfig.tileHeight))
    of.write("\n\n")

def writeRandomNets(of, benchConfig):
    # Die coordinates
    xLo = benchConfig.xLo
    yLo = benchConfig.yLo
    xHi = xLo + benchConfig.tileWidth * benchConfig.xNumGrids
    yHi = yLo + benchConfig.tileHeight * benchConfig.yNumGrids
    trackWidth = 280
    trackHeight = 280
    numTrackX = ( xHi - xLo )/ trackWidth - 1
    numTrackY = ( yHi - yLo )/ trackHeight - 1
    ptSet = set() # to make sure no pins are overlapping
    of.write("\nnum net ")
    of.write(str(benchConfig.numNets))
    for netIdx in range(0, benchConfig.numNets):
        of.write("\nn")
        of.write(str(netIdx))
        of.write(" ")
        of.write(str(netIdx))
        of.write(" ")
        of.write(str(benchConfig.numPins[netIdx]))
        of.write(" ")
        of.write(str(benchConfig.minWidth[0]))
        for pinIdx in range(0, benchConfig.numPins[netIdx]):
            pinLoc = Point()
            pinLoc.x = random.randint(0, numTrackX - 1)
            pinLoc.y = random.randint(0, numTrackY - 1)
            while (pinLoc in ptSet):
                pinLoc.x = random.randint(0, numTrackX - 1)
                pinLoc.y = random.randint(0, numTrackY - 1)
            ptSet.add(pinLoc)
            of.write("\n")
            of.write(str(int((pinLoc.x + 0.5 + xLo) * trackWidth)))
            of.write(" ")
            of.write(str(int((pinLoc.y + 0.5 + yLo) * trackHeight)))
            of.write(" 1")

def writeTempEnd(of):
    of.write("\n\n")
    of.write("net sym 0\n\n0")

def configGenerator():
    benchConfig = BenchConfig()
    #from TSMC 40 
    # Use 6 layers, M1 and all other Mx
    benchConfig.numLayers = 6
    #M1
    benchConfig.pitchWidth.append(280)
    benchConfig.pitchHeight.append(280)
    #Mx
    for i in range(0, 5):
        benchConfig.pitchWidth.append(280)
        benchConfig.pitchHeight.append(280)
    #direction
    for i in range(0, 6):
        benchConfig.minWidth.append(140)
        benchConfig.minSpacing.append(140)
        benchConfig.viaSpacing.append(140)
        if (i % 2 == 0):
            benchConfig.direction.append(0)
        else:
            benchConfig.direction.append(1)

    # Random generate design 
    tileWidthScale = random.randint(5, 5)
    tileHeightScale = random.randint(5, 5)
    benchConfig.tileWidth = 280 * tileWidthScale
    benchConfig.tileHeight = tileHeightScale * 280 

    benchConfig.xNumGrids = random.randint(100,100)
    benchConfig.yNumGrids = random.randint(100,100)

    benchConfig.xLo = random.randint(0, 0)
    benchConfig.yLo = random.randint(0, 0)

    benchConfig.capacityRatio = 0 # let it to be 1 for now

    #No cap adjust for now   

    #numNets = int(random.randint(10,15) * benchConfig.tileWidth  / 280 / 3)
    numNets = 1000
    print("num of nets: ", numNets)
    benchConfig.numNets = numNets

    for netIdx in range(0, numNets):
        numPin = numpy.random.choice(numpy.arange(2, 12), p=[0.05, 0.1, 0.1, 0.2, 0.2, 0.1, 0.1, 0.05, 0.05, 0.05])
        benchConfig.numPins.append(numPin)
    return benchConfig


#main

outputFile = sys.argv[1]

benchConfig = configGenerator()

with open(outputFile, "w") as of:
    writeDesignSettings(of, benchConfig)
    writeRandomNets(of, benchConfig)
    writeTempEnd(of)
