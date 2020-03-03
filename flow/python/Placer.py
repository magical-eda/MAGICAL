import magicalFlow
import IdeaPlaceExPy
import anaroutePy
import sys
sys.path.append('/home/local/eda10/jayliu/projects/develop/magical/magical/constraint_generation/python/')
import device_generation.basic as basic
import gdspy
import device_generation.glovar as glovar

class Placer(object):
    def __init__(self, magicalDB, cktIdx, dirname, gridStep, halfMetWid):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.debug = True
        self.cktIdx = cktIdx
        self.ckt = self.dDB.subCkt(cktIdx)
        self.placer = IdeaPlaceExPy.IdeaPlaceEx()
        self.dirname = dirname
        self.numIntervalNodes = self.ckt.numNodes() # without io pins
        self.gridStep = gridStep 
        self.halfMetWid = halfMetWid
        self.subShapeList = list()
    def run(self):
        self.dumpInput()
        self.symAxis = self.placer.solve(self.gridStep)
        self.processPlacementOutput()
    def dumpInput(self):
        self.placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
        self.placeParsePin()
        self.placeConnection()
        self.placer.readSymFile(self.dirname + self.ckt.name + '.sym')
        self.placeParseBoundary()
        if self.debug:
            gdspy.current_library = gdspy.GdsLibrary()
            self.tempCell = gdspy.Cell("FLOORPLAN")
        self.configureIoPinParameters()
    def configureIoPinParameters(self):
        self.placer.openVirtualPinAssignment()
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if (net.isIo()):
                self.placer.markIoNet(netIdx)
    def processPlacementOutput(self):
        #  Set Placement origin
        self.setPlaceOrigin()
        self.symAxis = int(self.symAxis - self.origin[0])
        self.readoutIoPins()
        self.writeoutPlacementResult()
    def readoutIoPins(self):
        self.iopinOffsetx =[]
        self.iopinOffsety = []
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if (net.isIo()):
                ioPinX = self.placer.iopinX(netIdx) - self.origin[0]
                ioPinY = self.placer.iopinY(netIdx) - self.origin[1]
                self.iopinOffsetx.append(ioPinX)
                self.iopinOffsety.append(ioPinY)
                #FIXME
                hWidth = 100
                hHeight = 100
                pdkLayer = 31 #M1
                dbLayer = self.tDB.pdkLayerToDb(pdkLayer)
                net.setIoShape(- hWidth, - hHeight,  hWidth,  hHeight)
                net.ioLayer = 1
                # add a new pin to the net
                iopinNodeIdx = self.ckt.allocateNode()
                iopinPinIdx = self.ckt.allocatePin()
                net.appendPinIdx(iopinPinIdx)
                iopinNode = self.ckt.node(iopinNodeIdx)
                iopinNode.isImpl = True
                iopinGraphIdx = self.dDB.allocateCkt() # this step will cause the change on the ckt vector. Potentially change the reference/pointer. So need to re-reference the ckt
                self.ckt = self.dDB.subCkt(self.cktIdx)
                iopinGraph = self.dDB.subCkt(iopinGraphIdx)
                iopinGraph.isImpl = True
                iopinGraph.allocateNet()
                subiopinNet = iopinGraph.net(0)
                iopinGraph.layout().insertRect(dbLayer,  - hWidth,  - hHeight,  hWidth,  hHeight)
                subiopinNet.setIoShape( - hWidth, - hHeight,  hWidth,  hHeight)
                subiopinNet.ioLayer = 1
                iopinNode.graphIdx = iopinGraphIdx
                iopinNode.appendPinIdx(iopinPinIdx)
                iopinPin = self.ckt.pin(iopinPinIdx)
                iopinPin.intNetIdx = 0
                iopinPin.nodeIdx = iopinNodeIdx
                iopinPin.netIdx = netIdx
                #iopinPin.appendPinIdx(iopinPinIdx)
        self.ckt = self.dDB.subCkt(self.cktIdx)
    def writeoutPlacementResult(self):
        # Write results to flow
        for nodeIdx in range(self.numIntervalNodes):
            cktNode = self.ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            x_offset = self.placer.xCellLoc(nodeIdx) - self.origin[0]
            y_offset = self.placer.yCellLoc(nodeIdx) - self.origin[1]
            print("node ", cktNode.name, x_offset, y_offset)
            cktNode.setOffset(x_offset, y_offset)
            self.ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
            print cktNode.name, self.placer.cellName(nodeIdx), x_offset, y_offset, "PLACEMENT"
            if self.debug:
                boundary = subCkt.layout().boundary()
                rect = gdspy.Rectangle((boundary.xLo+x_offset,boundary.yLo+y_offset), (boundary.xHi+x_offset,boundary.yHi+y_offset))
                text = gdspy.Text(cktNode.name,50,((boundary.xLo+boundary.xHi)/2+x_offset,(boundary.yLo+boundary.yHi)/2+y_offset),layer=100)
                self.tempCell.add(rect)
                self.tempCell.add(text)
        #Process io pins      
        for nodeIdx in range(self.numIntervalNodes, self.ckt.numNodes()):
            cktNode = self.ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            x_offset = self.iopinOffsetx[nodeIdx - self.numIntervalNodes]
            y_offset = self.iopinOffsety[nodeIdx - self.numIntervalNodes]
            cktNode.setOffset(x_offset, y_offset)
            self.ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
        # write guardring using gdspy
        if self.cktNeedSub(self.cktIdx):
            print "Adding GuardRing to Cell"
            bBox = self.ckt.layout().boundary()
            # Leave additional 80nm spacing
            grCell, subPin = basic.sub_GR([bBox.xLo/1000.0-0.08, bBox.yLo/1000.0-0.08], [bBox.xHi/1000.0+0.08, bBox.yHi/1000.0+0.08], [-self.halfMetWid/1000.0,-self.halfMetWid/1000.0])
            self.addPycell(self.ckt.layout(), grCell)
            bBox = self.ckt.layout().boundary()
            self.subShape(subPin)
        # Output placement result
        magicalFlow.writeGdsLayout(self.cktIdx, self.dirname + self.ckt.name + '.place.gds', self.dDB, self.tDB)
        self.origin = [0,0]
        if self.debug:
            gdspy.write_gds(self.dirname+self.ckt.name+'.floorplan.gds', [self.tempCell], unit=1.0e-9, precision=1.0e-9)


    def placeConnection(self):
        """
        @brief add pin connection info to placer
        """
        fileName = self.dirname + self.ckt.name + '.con'
        if self.debug:
            outFile = open(fileName, 'w')
            outFile.write("%d\n" % self.ckt.numNets())
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            dbNetIdx = self.placer.allocateNet()
            assert netIdx == dbNetIdx, "placeConnection, netIdx == dbNetIdx"
            if self.debug:
                outFile.write("%d\n" % net.numPins())
            for pinIdx in range(net.numPins()):
                pinidxidx = net.pinIdx(pinIdx)
                pin = self.ckt.pin(pinidxidx)
                nodeIdx = pin.nodeIdx
                if not pin.valid:
                    if self.debug:
                        outFile.write("-1\n")
                    continue
                if self.placer.pinIdx(nodeIdx, pin.intNetIdx) < pow(2,32)-1:
                    self.placer.addPinToNet(self.placer.pinIdx(nodeIdx, pin.intNetIdx), netIdx)
                    if self.debug:
                        outFile.write("%d\n" % self.placer.pinIdx(nodeIdx, pin.intNetIdx))
                else:
                    device_type = self.dDB.subCkt(self.ckt.node(nodeIdx).graphIdx).implType
                    if device_type == magicalFlow.ImplTypePCELL_Nch:
                        assert pin.intNetIdx == 3, "placeConnection, sub not NMOS BODY pin"
                    elif device_type == magicalFlow.ImplTypePCELL_Res:
                        assert pin.intNetIdx == 2, "placeConnection, sub not RES BODY pin"
                    else:
                        assert False, "placeConnection, sub not NMOS or RES device"
                    if self.debug:
                        outFile.write("-1\n")

    def placeParsePin(self):
        """
        @brief parse pin and cell for placer
        """
        fileName = self.dirname + self.ckt.name + '.pin'
        if self.debug:
            outFile = open(fileName, 'w') #
            outFile.write("%d\n" % self.ckt.numNodes())
        for nodeIdx in range(self.ckt.numNodes()):
            cellIdx = self.placer.allocateCell()
            assert nodeIdx == cellIdx, "placeParsePin, nodeIdx != ckdIdx"
            node = self.ckt.node(nodeIdx)
            self.placer.setCellName(nodeIdx, node.name)
            subCkt = self.dDB.subCkt(node.graphIdx)
            if self.debug:
                outFile.write('%s %d\n' % (node.name,subCkt.numNets())) #
            for netIdx in range(subCkt.numNets()):
                net = subCkt.net(netIdx)
                shape = net.ioShape()
                layer = net.ioLayer
                # pinIdx not exists, unsigned int max:pow(2,32)-1
                if layer > 10:
                    outFile.write("-1\n")
                    continue
                pinIdx = self.placer.allocatePin(nodeIdx)
                assert pinIdx == self.placer.pinIdx(nodeIdx, netIdx), "placeParsePin, pin insertion error"
                self.placer.addPinShape(pinIdx, shape.xLo, shape.yLo, shape.xHi, shape.yHi)
                if self.debug:
                    outFile.write("%d %d %d %d\n" % (shape.xLo, shape.yLo, shape.xHi, shape.yHi))

    def placeParseBoundary(self):
        fileName = self.dirname + self.ckt.name + '.bound'
        if self.debug:
            outFile = open(fileName, 'w') #
            outFile.write("%d\n" % self.ckt.numNodes())
        for nodeIdx in range(self.ckt.numNodes()):
            cktNode = self.ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            self.placer.addCellShape(nodeIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
            if self.debug:
                outFile.write("%d %d %d %d\n" % (bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi))

    def setPlaceOrigin(self):
        self.origin = [self.placer.xCellLoc(0), self.placer.yCellLoc(0)]
        for nodeIdx in range(self.numIntervalNodes):
            if self.origin[0] > self.placer.xCellLoc(nodeIdx):
                self.origin = [self.placer.xCellLoc(nodeIdx), self.placer.yCellLoc(nodeIdx)]
            elif self.origin[0] == self.placer.xCellLoc(nodeIdx) and self.origin[1] > self.placer.yCellLoc(nodeIdx):
                self.origin = [self.placer.xCellLoc(nodeIdx), self.placer.yCellLoc(nodeIdx)]
    def cktNeedSub(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            _, isPsub, _ = self.netPinCount(ckt, net)
            if isPsub:
                return True
        return False

    @staticmethod
    def netPinCount(ckt, net):
        netPsub = 0
        netNwell = 0
        pinCount = 0
        for pinId in range(net.numPins()):
            pinIdx = net.pinIdx(pinId)
            pin = ckt.pin(pinIdx)
            if pin.pinType == magicalFlow.PinType.PSUB:
                netPsub = 1
            elif pin.pinType == magicalFlow.PinType.NWELL:
                netNwell = 1
                pinCount += 1
            else:
                pinCount += 1
        return pinCount + netPsub, bool(netPsub), bool(netNwell)

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

    def subShape(self, subPin):
        shape = subPin.normalize_shape()
        # Only the lower metal for now
        self.subShapeList.append(shape[1:5])
        
