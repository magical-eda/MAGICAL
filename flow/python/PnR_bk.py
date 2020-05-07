##
# @file PnR.py
# @author Keren Zhu, Mingjie Liu
# @date 07/07/2019
# @brief The class for implementing one layout for a circuit
#
import magicalFlow
import IdeaPlaceExPy
import anaroutePy
import sys
sys.path.append('/home/local/eda10/jayliu/projects/develop/magical/magical/constraint_generation/python/')
import device_generation.basic as basic
import Router

class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.dDB.findRootCkt()
        self.rootCktIdx = self.dDB.rootCktIdx()

    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        isTopCkt = (self.rootCktIdx == cktIdx)
        cirname = self.dDB.subCkt(cktIdx).name
        #self.writeBlockFile(cktIdx, dirname+cirname+'.block')
        #self.writeConnectionFile(cktIdx, dirname+cirname+'.connection')
        #self.writeNetFile(cktIdx, dirname+cirname+'.net')
        # The .pin file is not needed now and might be wrong due to flipping symmetry cells
        #self.writePinFile(cktIdx, dirname+cirname+'.pin')
        self.runPlace(cktIdx, dirname)
        self.runRoute(cktIdx, dirname, isTopCkt)
        Router.Router(self.mDB).readBackDumbFile(dirname+cirname+'.ioPin', cktIdx)
        self.dDB.subCkt(cktIdx).isImpl = True

    def runPlace(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        cktname = ckt.name
        placer = IdeaPlaceExPy.IdeaPlaceEx()
        placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
        self.placeParsePin(placer, cktIdx)
        self.placeConnection(placer, cktIdx)
        placer.readSymFile(dirname + cktname + '.sym')
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            placer.addCellShape(nodeIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
        placer.solve()
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            x_offset = placer.xCellLoc(nodeIdx)
            y_offset = placer.yCellLoc(nodeIdx)
            cktNode.setOffset(x_offset, y_offset)
            ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
        # write guardring using gdspy
        if self.cktNeedSub(cktIdx):
            print "Adding GuardRing to Cell"
            bBox = ckt.layout().boundary()
            # Leave additional 80nm spacing
            grCell = basic.sub_GR([bBox.xLo/1000.0-0.08, bBox.yLo/1000.0-0.08], [bBox.xHi/1000.0+0.08, bBox.yHi/1000.0+0.08])
            self.addPycell(ckt.layout(), grCell)
            bBox = ckt.layout().boundary()
            self.subShape(bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
        magicalFlow.writeGdsLayout(cktIdx, dirname + cktname + '.place.gds', self.dDB, self.tDB)
            
    def runPlace_file(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        cktname = ckt.name
        placer = IdeaPlaceExPy.IdeaPlaceEx()
        placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
        placer.readPinFile(dirname + cktname + '.pin')
        placer.readConnectionFile(dirname + cktname + '.connection')
        placer.readSymFile(dirname + cktname + '.sym')
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            cellIdx = placer.cellIdxName(cktNode.name)
            placer.addCellShape(cellIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
        placer.solve()
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            cellIdx = placer.cellIdxName(cktNode.name)
            x_offset = placer.xCellLoc(cellIdx)
            y_offset = placer.yCellLoc(cellIdx)
            cktNode.setOffset(x_offset, y_offset)
            ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
        if self.cktNeedSub(cktIdx):
            bBox = ckt.layout().boundary()
            grCell = basic.sub_GR([bBox.xLo/1000.0-0.08, bBox.yLo/1000.0-0.08], [bBox.xHi/1000.0+0.08, bBox.yHi/1000.0+0.08])
            self.addPycell(ckt.layout(), grCell)
        magicalFlow.writeGdsLayout(cktIdx, dirname + cktname + '.place.gds', self.dDB, self.tDB)
        boundary = ckt.layout().boundary()
        # TODO: Remove in future
        if self.cktNeedSub(cktIdx):
            outFile = dirname + cktname + '.sub'
            self.writeSub(boundary.xLo, boundary.yLo, boundary.xHi, boundary.yHi, outFile)

    def addPycell(self, layout, pyCell):
        polygons = pyCell.get_polygons(True)
        layers = polygons.keys()
        for layer in layers:
            layerIdx = self.tDB.pdkLayerToDb(layer[0])
            datatype = layer[1]
            polyList = polygons[layer]
            for poly in polyList:
                xLo = int(round(poly[0][0]*200))*5
                yLo = int(round(poly[0][1]*200))*5
                xHi = int(round(poly[2][0]*200))*5
                yHi = int(round(poly[2][1]*200))*5
                rectIdx = layout.insertRect(layerIdx, xLo, yLo, xHi, yHi)
                if datatype != 0:
                    layout.setRectDatatype(layerIdx, rectIdx, datatype)

    def subShape(self, xlo, ylo, xhi, yhi):
        self.subShapeList = list()
        xlo = xlo + 50
        ylo = ylo + 50
        xhi = xhi - 50
        yhi = yhi - 50
        w = 170
        self.subShapeList.append([xlo,ylo])
        self.subShapeList.append([xhi,ylo+w])
        #self.sub.append([xhi,ylo])
        #self.sub.append([xhi,yhi])
        #self.sub.append([xlo,yhi])
        #self.sub.append([xlo,ylo+w])
        #self.sub.append([xlo+w,ylo+w])
        #self.sub.append([xlo+w,yhi-w])
        #self.sub.append([xhi-w,yhi-w])
        #self.sub.append([xhi-w,ylo+w])
        #self.sub.append([xlo,ylo+w])

    def writeSub(self, xlo, ylo, xhi, yhi, outFile):
        xlo = xlo + 50
        ylo = ylo + 50
        xhi = xhi - 50
        yhi = yhi - 50
        w = 170
        fout = open(outFile, "w")
        fout.write("1 ")
        fout.write("%d %d "% (xlo, ylo))
        fout.write("%d %d "% (xhi, ylo))
        fout.write("%d %d "% (xhi, yhi))
        fout.write("%d %d "% (xlo, yhi))
        fout.write("%d %d "% (xlo, ylo+w))
        fout.write("%d %d "% (xlo+w, ylo+w))
        fout.write("%d %d "% (xlo+w, yhi-w))
        fout.write("%d %d "% (xhi-w, yhi-w))
        fout.write("%d %d "% (xhi-w, ylo+w))
        fout.write("%d %d \n"% (xlo, ylo+w))
        fout.close()

    def runRoute(self, cktIdx, dirname, top=False):
        ckt = self.dDB.subCkt(cktIdx)
        router = anaroutePy.AnaroutePy()
        placeFile = dirname + ckt.name + '.place.gds'
        router.parseLef('/home/unga/jayliu/projects/inputs/tcbn40lpbwp_10lm7X2ZRDL.lef')
        router.parseTechfile('/home/unga/jayliu/projects/inputs/techfile')
        router.parseGds(placeFile)
        pinNameIdx = 0
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            router.addNet(net.name)
            grPinCount, isPsub = self.netPinCount(ckt, net)
            for pinId in range(net.numPins()):
                pinIdx = net.pinIdx(pinId)
                pin = ckt.pin(pinIdx)
                conNode = pin.nodeIdx
                conNet = pin.intNetIdx
                conCkt = self.dDB.subCkt(ckt.node(conNode).graphIdx)
                if pin.pinType == magicalFlow.PinType.PSUB:
                    assert net.isSub
                    continue
                # Router starts as 0 with M1
                conLayer = conCkt.net(conNet).ioLayer - 1
                conShape = self.adjustIoShape(conCkt.net(conNet).ioShape(), ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                router.addPin(str(pinNameIdx))
                router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                router.addShape2Pin(pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                pinNameIdx += 1
            if isPsub:
                assert self.cktNeedSub(cktIdx)
                router.addPin(str(pinNameIdx))
                router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                router.addShape2Pin(pinNameIdx, 0, self.subShapeList[0][0]*2, self.subShapeList[0][1]*2, self.subShapeList[1][0]*2, self.subShapeList[1][1]*2)
                pinNameIdx += 1
        router.parseSymNet(dirname+ckt.name+'.symnet')
        router.solve()
        router.writeLayoutGds(placeFile, dirname+ckt.name+'.route.gds', True)
        router.writeDumb(placeFile, dirname+ckt.name+'.ioPin') 
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')

    def runRoute_file(self, cktIdx, dirname, top=False):
        wellFinished = " false"
        subFinished = " false"
        cirname = self.dDB.subCkt(cktIdx).name
        if self.cktNeedSub(cktIdx):
            self.readSub(dirname + cirname + ".sub")
            subFinished = " true"
        self.writeGR(cktIdx, dirname + cirname + ".ROUTING_INPUT.gr")
        self.writeGR(cktIdx, dirname + "ROUTING_INPUT.gr")
        if top:
            script_name = "run_route_hao_top.sh "
        else:
            script_name = "run_route_hao.sh "
        cmd = "source /home/unga/jayliu/projects/develop/magical/magical/install/test/" + script_name + cirname + " ../../inputs/techfile ../../inputs/techfile.simple ../../inputs/spacing.rules ../../inputs/width_area.rules ../../inputs/enclosure.rules ../../inputs/M1_NW_x2.gds ../../inputs/tcbn40lpbwp_10lm7X2ZRDL.lef " + dirname + wellFinished + subFinished# + " &>/dev/null"
        subprocess.call(cmd, shell=True)
        ckt = self.dDB.subCkt(cktIdx)
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')

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

    def placeConnection(self, placer, cktIdx):
        """
        @brief add pin connection info to placer
        """
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            dbNetIdx = placer.allocateNet()
            assert netIdx == dbNetIdx, "placeConnection, netIdx == dbNetIdx"
            for pinIdx in range(net.numPins()):
                pinidxidx = net.pinIdx(pinIdx)
                pin = ckt.pin(pinidxidx)
                nodeIdx = pin.nodeIdx
                if placer.pinIdx(nodeIdx, pin.intNetIdx) < pow(2,32)-1:
                    placer.addPinToNet(placer.pinIdx(nodeIdx, pin.intNetIdx), netIdx)
                else:
                    assert pin.intNetIdx == 3, "Should be a BODY pin"
                    assert self.dDB.subCkt(ckt.node(nodeIdx).graphIdx)
                    print nodeIdx, pin.intNetIdx, "Should be NMOS BODY pin"

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

    def placeParsePin(self, placer, cktIdx):
        """
        @brief parse pin and cell for placer
        """
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            cellIdx = placer.allocateCell()
            assert nodeIdx == cellIdx, "placeParsePin, nodeIdx != ckdIdx"
            node = ckt.node(nodeIdx)
            placer.setCellName(nodeIdx, node.name)
            subCkt = self.dDB.subCkt(node.graphIdx)
            for netIdx in range(subCkt.numNets()):
                net = subCkt.net(netIdx)
                shape = net.ioShape()
                layer = net.ioLayer
                # pinIdx not exists, unsigned int max:pow(2,32)-1
                if layer > 10:
                    continue
                pinIdx = placer.allocatePin(nodeIdx)
                assert pinIdx == placer.pinIdx(nodeIdx, netIdx), "placeParsePin, pin insertion error"
                placer.addPinShape(pinIdx, shape.xLo, shape.yLo, shape.xHi, shape.yHi)

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
            ##### TODO DIRTY FIX FOR PIN NEED TO BE BETTER
            #fout.write("%s %d %d 1 1 1\n"%(net.name, netIdx, grPinCount))
            fout.write("%s %d %d 1\n"%(net.name, netIdx, grPinCount))
            if isPsub:
                fout.write(self.psub)
            for pinId in range(numPin):
                pinIdx = net.pinIdx(pinId)
                pin = ckt.pin(pinIdx)
                conNode = pin.nodeIdx
                conNet = pin.intNetIdx
                conCkt = self.dDB.subCkt(ckt.node(conNode).graphIdx)
                flipFlag = ckt.node(conNode).flipVertFlag
                if pin.pinType == magicalFlow.PinType.PSUB:
                    assert net.isSub
                    continue
                conShape = self.adjustIoShape(conCkt.net(conNet).ioShape(), ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                xLo = int(conShape[0]) 
                yLo = int(conShape[1])
                xHi = int(conShape[2])
                yHi = int(conShape[3])
                fout.write(str(conCkt.net(conNet).ioLayer))
                fout.write(" %s\n" % self.pinToStr(xLo,yLo,xHi,yHi))

    @staticmethod
    def flipPin(xLo, xHi, symAxis_2):
        xLo_s = symAxis_2 - xHi
        xHi_s = symAxis_2 - xLo
        return xLo_s, xHi_s

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
    def adjustIoShape(ioShape, offset, cellBBox, flipPin):
        if flipPin:
            symAxis = (cellBBox.xLo + cellBBox.xHi)
            xLo, xHi = PnR.flipPin(ioShape.xLo, ioShape.xHi, symAxis)
        else:
            xLo = ioShape.xLo
            xHi = ioShape.xHi
        xLo = xLo + offset.x #- cellBBox.xLo ?????
        xHi = xHi + offset.x #- cellBBox.xLo ?????
        yLo = ioShape.yLo + offset.y #- cellBBox.yLo
        yHi = ioShape.yHi + offset.y #- cellBBox.yLo
        return [xLo, yLo, xHi, yHi]

    @staticmethod
    def pinToStr(xLo, yLo, xHi, yHi):
        return str(xLo)+" "+str(yLo)+" "+str(xHi)+" "+str(yLo)+" "+str(xHi)+" "+str(yHi)+" "+str(xLo)+" "+str(yHi)
 
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
