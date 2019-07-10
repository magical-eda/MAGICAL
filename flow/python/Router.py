##
# @file Router.py
# @author Keren Zhu
# @date 07/09/2019
# @brief The class for interfacing routing
#

import MagicalDB
import magicalFlow

class Router(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.tDB = magicalDB.techDB
        self.dDB = magicalDB.designDB.db

    def readBackDumbFile(self, filename, cktIdx):
        """
        @brief read back the for-integration dumb file 
        """
        ckt = self.dDB.subCkt(cktIdx)
        gdsData = ckt.gdsData()
        with open(filename)  as fin:
            lines = fin.readlines()
            bboxLine = lines[0].split()
            xLo = int(bboxLine[0])
            yLo = int(bboxLine[1])
            xHi = int(bboxLine[2])
            yHi = int(bboxLine[3])
            gdsData.setBBox(xLo, yLo, xHi, yHi)
            for lineIdx in range(1, len(lines)):
                line = lines[lineIdx].split()
                netIdx = lineIdx - 1
                net = ckt.net(netIdx)
                layer = int(line[1])
                xLo = int(line[2])
                yLo = int(line[3])
                xHi = int(line[4])
                yHi = int(line[5])
                net.setIoShape(xLo, yLo, xHi, yHi)
                net.ioLayer = layer

