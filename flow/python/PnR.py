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
        self.runPlace(cktIdx, dirname)
        self.runRoute(cktIdx, dirname, isTopCkt)
        self.dDB.subCkt(cktIdx).isImpl = True
            
    def runPlace(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        cktname = ckt.name
        placer = IdeaPlaceExPy.IdeaPlaceEx()
        placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
        self.placeParsePin(placer, ckt)
        self.placeConnection(placer, ckt)
        placer.readSymFile(dirname + cktname + '.sym')
        self.placeParseBoundary(placer, ckt)
        placer.solve()
        # Write results to flow
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
        # Output placement result
        magicalFlow.writeGdsLayout(cktIdx, dirname + cktname + '.place.gds', self.dDB, self.tDB)

    def runRoute(self, cktIdx, dirname, top=False):
        ckt = self.dDB.subCkt(cktIdx)
        router = anaroutePy.AnaroutePy()
        placeFile = dirname + ckt.name + '.place.gds'
        router.parseLef('/home/unga/jayliu/projects/inputs/tcbn40lpbwp_10lm7X2ZRDL.lef')
        router.parseTechfile('/home/unga/jayliu/projects/inputs/techfile')
        router.parseGds(placeFile)
        self.routeParsePin(router, cktIdx)        
        router.parseSymNet(dirname+ckt.name+'.symnet')
        router.solve()
        router.writeLayoutGds(placeFile, dirname+ckt.name+'.route.gds', True)
        router.writeDumb(placeFile, dirname+ckt.name+'.ioPin') 
        # Read results to flow
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')
        Router.Router(self.mDB).readBackDumbFile(dirname+ckt.name+'.ioPin', cktIdx)

    def placeConnection(self, placer, ckt):
        """
        @brief add pin connection info to placer
        """
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

    def placeParsePin(self, placer, ckt):
        """
        @brief parse pin and cell for placer
        """
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

    def placeParseBoundary(self, placer, ckt):
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            placer.addCellShape(nodeIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
            
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
        # Only the lower metal for now
        self.subShapeList.append([xlo,ylo])
        self.subShapeList.append([xhi,ylo+w])
        
    def routeParsePin(self, router, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
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
        xLo = xLo + offset.x 
        xHi = xHi + offset.x 
        yLo = ioShape.yLo + offset.y 
        yHi = ioShape.yHi + offset.y 
        return [xLo, yLo, xHi, yHi]

