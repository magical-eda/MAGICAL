##
# @file PnR.py
# @author Keren Zhu, Mingjie Liu
# @date 07/07/2019
# @brief The class for implementing one layout for a circuit
#
import subprocess
import Router
import magicalFlow

class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB

    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        cirname = self.dDB.subCkt(cktIdx).name
        self.writeBlockFile(cktIdx, dirname+cirname+'.block')
        self.writeConnectionFile(cktIdx, dirname+cirname+'.connection')
        self.writeNetFile(cktIdx, dirname+cirname+'.net')
        self.writeOffsetFile(cktIdx, dirname+cirname+'.offset')
        self.writePinFile(cktIdx, dirname+cirname+'.pin')
        self.runPlace(cktIdx, dirname)
        self.runRoute(cktIdx, dirname)
        Router.Router(self.mDB).readBackDumbFile(cirname+'.route.gds.dumb', cktIdx)
        self.dDB.subCkt(cktIdx).isImpl = True

    def runMagical(self, cktIdx, dirname):
        wellFinished = " true"
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            if node.isLeaf():
                assert(False)
            subCktIdx = node.graphIdx
            subCkt = self.dDB.subCkt(subCktIdx)
            if not magicalFlow.isImplTypeDevice(subCkt.implType):
                wellFinished = " false"
                break
        cirname = self.dDB.subCkt(cktIdx).name
        cmd = "source /home/unga/jayliu/projects/develop/magical/magical/install/test/run.sh " + cirname + " ../../inputs/techfile ../../inputs/techfile.simple ../../inputs/spacing.rules ../../inputs/width_area.rules ../../inputs/enclosure.rules ../../inputs/M1_NW_x2.gds ../../inputs/tcbn40lpbwp_10lm7X2ZRDL.lef " + dirname + wellFinished 
        subprocess.call(cmd, shell=True)

    def runPlace(self, cktIdx, dirname):
        wellFinished = " false"
        subFinished = " false"
        if self.cktNeedSub(cktIdx):
            subFinished = " true"
        cirname = self.dDB.subCkt(cktIdx).name
        cmd = "source /home/unga/jayliu/projects/develop/magical/magical/install/test/run_place.sh " + cirname + " ../../inputs/techfile ../../inputs/techfile.simple ../../inputs/spacing.rules ../../inputs/width_area.rules ../../inputs/enclosure.rules ../../inputs/M1_NW_x2.gds ../../inputs/tcbn40lpbwp_10lm7X2ZRDL.lef " + dirname + wellFinished + subFinished 
        print cmd
        subprocess.call(cmd, shell=True)
        self.readPlace(cktIdx, dirname + "result_legal_detail.txt")

    def runRoute(self, cktIdx, dirname):
        wellFinished = " false"
        subFinished = " false"
        cirname = self.dDB.subCkt(cktIdx).name
        if self.cktNeedSub(cktIdx):
            self.readSub(dirname + cirname + ".sub")
            subFinished = " true"
        self.writeGR(cktIdx, dirname + cirname + ".ROUTING_INPUT.gr")
        self.writeGR(cktIdx, dirname + "ROUTING_INPUT.gr")
        cmd = "source /home/unga/jayliu/projects/develop/magical/magical/install/test/run_route.sh " + cirname + " ../../inputs/techfile ../../inputs/techfile.simple ../../inputs/spacing.rules ../../inputs/width_area.rules ../../inputs/enclosure.rules ../../inputs/M1_NW_x2.gds ../../inputs/tcbn40lpbwp_10lm7X2ZRDL.lef " + dirname + wellFinished + subFinished 
        subprocess.call(cmd, shell=True)

    def writeBlockFile(self, cktIdx, filename):
        """
        @brief write .block file
        """
        ckt = self.dDB.subCkt(cktIdx)
        fout = open(filename, "w")
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write(" ")
            fout.write(str(subCkt.gdsData().bbox().xLen()/10))
            fout.write(" ")
            fout.write(str(subCkt.gdsData().bbox().yLen()/10))
            fout.write("\n")

    def writeConnectionFile(self, cktIdx, filename):
        """
        @brief write .connection file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            fout.write(net.name)
            fout.write(" ")
            for pinIdx in range(net.numPins()):
                pinidxidx = net.pinIdx(pinIdx)
                pin = ckt.pin(pinidxidx)
                nodeIdx = pin.nodeIdx
                node = ckt.node(nodeIdx)
                fout.write(node.name)
                fout.write(" ")
                fout.write(str(pin.intNetIdx))
                fout.write(" ")
            fout.write("\n")

    def writeNetFile(self, cktIdx, filename):
        """
        @brief write .net file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            fout.write(net.name)
            fout.write(" ")
            for pinIdx in range(net.numPins()):
                pinidxidx = net.pinIdx(pinIdx)
                pin = ckt.pin(pinidxidx)
                nodeIdx = pin.nodeIdx
                node = ckt.node(nodeIdx)
                fout.write(node.name)
                fout.write(" 0 0 ")
            fout.write("\n")

    def writeOffsetFile(self, cktIdx, filename):
        """
        @brief write .offset file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write(" ")
            xLo = subCkt.gdsData().bbox().xLo
            yLo = subCkt.gdsData().bbox().yLo
            fout.write(str(float(xLo) / 1000))
            fout.write(" ")
            fout.write(str(float(yLo) / 1000))
            fout.write("\n")

    def writePinFile(self, cktIdx, filename):
        """
        @brief write .pin file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write("\n")
            offsetXLo = float(subCkt.gdsData().bbox().xLo) / 1000
            offsetYLo = float(subCkt.gdsData().bbox().yLo) / 1000
            for netIdx in range(subCkt.numNets()):
                net = subCkt.net(netIdx)
                shape = net.ioShape()
                layer = net.ioLayer
                if layer > 10:
                    continue
                xLo = float(shape.xLo)  / 1000
                yLo = float(shape.yLo)  / 1000
                xHi = float(shape.xHi)  / 1000
                yHi = float(shape.yHi)  / 1000
                fout.write(str(netIdx))
                fout.write("    1\n ")
                fout.write("M")
                fout.write(str(layer))
                fout.write("    ((")
                fout.write(str(xLo - offsetXLo))
                fout.write(" ")
                fout.write(str(yLo - offsetYLo))
                fout.write(") (")
                fout.write(str(xHi - offsetXLo))
                fout.write(" ")
                fout.write(str(yHi - offsetYLo))
                fout.write("))\n")

    def readPlace(self, cktIdx, filename):
        ckt = self.dDB.subCkt(cktIdx)
        nameIdxDict = dict()
        for nodeIdx in range(ckt.numNodes()):
            nameIdxDict[ckt.node(nodeIdx).name] = nodeIdx
        with open(filename) as fin:
            lines = fin.readlines()
            for line in lines:
                line = line.split()
                nodeIdx = nameIdxDict[line[0]]
                x = int(float(line[1])* 1000)
                y = int(float(line[2])* 1000)
                ckt.node(nodeIdx).setOffset(x, y)

    def readSub(self, filename):
        with open(filename) as fin:
            lines = fin.readlines()
            self.psub = lines[0]

    def writeGR(self, cktIdx, filename):
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        fout.write("num net %d\n"%ckt.numNets())
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            numPin = net.numPins()
            grPinCount, isPsub = self.netPinCount(ckt, net)
            fout.write("%s %d %d 1\n"%(net.name, netIdx, grPinCount))
            if isPsub:
                fout.write(self.psub)
            for pinId in range(numPin):
                pinIdx = net.pinIdx(pinId)
                pin = ckt.pin(pinIdx)
                conNode = pin.nodeIdx
                conNet = pin.intNetIdx
                conCkt = self.dDB.subCkt(ckt.node(conNode).graphIdx)
                if pin.pinType == magicalFlow.PinType.PSUB:
                    assert net.isSub
                    continue
                conShape = self.adjustIoShape(conCkt.net(conNet).ioShape(), ckt.node(conNode).offset(), conCkt.gdsData().bbox())
                xLo = int(conShape[0]) 
                yLo = int(conShape[1])
                xHi = int(conShape[2])
                yHi = int(conShape[3])
                fout.write(str(conCkt.net(conNet).ioLayer))
                fout.write(" %s\n" % self.pinToStr(xLo,yLo,xHi,yHi))

    def cktNeedSub(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            _, isPsub = self.netPinCount(ckt, net)
            if isPsub:
                return True
        return False

    @staticmethod
    def netPinCount(ckt, net):
        netPsub = 0
        pinCount = 0
        for pinId in range(net.numPins()):
            pinIdx = net.pinIdx(pinId)
            pin = ckt.pin(pinIdx)
            if pin.pinType == magicalFlow.PinType.PSUB:
                netPsub = 1
            else:
                pinCount += 1
        return pinCount + netPsub, bool(netPsub)

    @staticmethod
    def adjustIoShape(ioShape, offset, cellBBox):
        xLo = ioShape.xLo + offset.x - cellBBox.xLo
        xHi = ioShape.xHi + offset.x - cellBBox.xLo
        yLo = ioShape.yLo + offset.y - cellBBox.yLo
        yHi = ioShape.yHi + offset.y - cellBBox.yLo
        return [xLo, yLo, xHi, yHi]

    @staticmethod
    def pinToStr(xLo, yLo, xHi, yHi):
        return str(xLo)+" "+str(yLo)+" "+str(xHi)+" "+str(yLo)+" "+str(xHi)+" "+str(yHi)+" "+str(xLo)+" "+str(yHi)
