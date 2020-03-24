import magicalFlow
import IdeaPlaceExPy
import anaroutePy
import sys
sys.path.append('/home/local/eda10/jayliu/projects/develop/magical/magical/constraint_generation/python')
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
        self.params = magicalDB.params
    def run(self):
        self.useIoPin = True
        #if (self.dDB.rootCktIdx() ==  self.cktIdx):
        #    self.useIoPin = False
        self.dumpInput()
        self.symAxis = self.placer.solve(self.gridStep)
        self.processPlacementOutput()
    def dumpInput(self):
        self.placer.readTechSimpleFile(self.params.simple_tech_file)
        self.placeParsePin()
        self.placeConnection()
        self.placer.readSymFile(self.dirname + self.ckt.name + '.sym') # FIXME: use in memory interface
        self.placeParseBoundary()
        if self.debug:
            gdspy.current_library = gdspy.GdsLibrary()
            self.tempCell = gdspy.Cell("FLOORPLAN")
        self.configureIoPinParameters()
        self.placer.readSymNetFile(self.dirname + self.ckt.name + '.symnet') # FIXME: use in memory interface
    def configureIoPinParameters(self):
        if self.useIoPin == False:
            self.placer.closeVirtualPinAssignment()
            return
        self.placer.openVirtualPinAssignment()
        self.placer.setIoPinBoundaryExtension(3 * 1 * self.gridStep)
        self.placer.setIoPinInterval(3 * 2 * self.gridStep)
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if (net.isIo() and (not net.isPower())):
                self.placer.markIoNet(netIdx)
            if net.isVdd():
                self.placer.markAsVddNet(netIdx)
            if net.isVss():
                self.placer.markAsVssNet(netIdx)
    def processPlacementOutput(self):
        #  Set Placement origin
        self.setPlaceOrigin()
        print("origin, ", self.origin[0], self.origin[1])
        self.symAxis = int(self.symAxis - self.origin[0])
        self.readoutIoPins()
        self.writeoutPlacementResult()
    def readoutIoPins(self):
        self.iopinOffsetx =[]
        self.iopinOffsety = []
        if self.useIoPin == False:
            return
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if (net.isIo() and (not net.isPower())):
                ioPinX = self.placer.iopinX(netIdx) - self.origin[0]
                ioPinY = self.placer.iopinY(netIdx) - self.origin[1]
                self.iopinOffsetx.append(ioPinX)
                self.iopinOffsety.append(ioPinY)
                #FIXME
                if (self.placer.isIoPinVertical(netIdx)):
                    metals = [
                            [- 65, -self.gridStep  - 70 - 30, 65, self.gridStep  + 70 + 30]
                            ]
                    metalPkdLayers = [31]
                    metalIoLayers = [1]
                else:
                    metals = [
                            [-self.gridStep  - 70 - 30, - 65, self.gridStep  + 70 + 30, 65]
                            ]
                    metalPkdLayers = [31]
                    metalIoLayers = [1]
                cuts = []
                cutsPdkLayers = []
                # add a new pin to the net
                self.addIoPinToNet(netIdx, ioPinX, ioPinY, metals, metalPkdLayers, cuts, cutsPdkLayers)
        self.ckt = self.dDB.subCkt(self.cktIdx)
    def upscaleBBox(self, gridStep, ckt, origin):
        """
        @brief for legalize the boundary box after the routing. The routing wire might change the boundary of placement, so that the bounding box need to be adjust to multiple of grid step
        """
        bBox = ckt.layout().boundary()
        xLoLen = origin[0] - bBox.xLo 
        yLoLen = origin[1] - bBox.yLo 
        xHiLen = bBox.xHi - origin[0] 
        yHiLen = bBox.yHi - origin[1] 
        print("adjust bbox ", xLoLen, yLoLen, xHiLen, yHiLen)
        xLoLen += gridStep - (xLoLen % gridStep)
        xHiLen += gridStep - (xHiLen % gridStep)
        yLoLen += gridStep - (yLoLen % gridStep)
        yHiLen += gridStep - (yHiLen % gridStep)
        ckt.layout().setBoundary( origin[0] - xLoLen,
                                  origin[1] - yLoLen,
                                  origin[0] + xHiLen,
                                  origin[1] + yHiLen)

    def writeoutPlacementResult(self):
        # Write results to flow
        self.initPowerPins()
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
        cktBoundaryBox = self.ckt.layout().boundary()
        #Process io pins      
        if self.useIoPin:
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
            print("origin", self.origin[0], self.origin[1])
            # Leave additional 80nm spacing
            grCell, subPin = basic.sub_GR([cktBoundaryBox.xLo/1000.0-0.08, cktBoundaryBox.yLo/1000.0-0.08], [cktBoundaryBox.xHi/1000.0+0.08, cktBoundaryBox.yHi/1000.0+0.08], [self.origin[0]-self.halfMetWid/1000.0,self.origin[1]-self.halfMetWid/1000.0])
            self.addPycell(self.ckt.layout(), grCell)
            bBox = self.ckt.layout().boundary()
            self.subShape(subPin)
        # Power stripes
        if self.useIoPin:
            existingNodes = self.ckt.numNodes()
            self.addPowerStripe(cktBoundaryBox)
            for nodeIdx in range(existingNodes, self.ckt.numNodes()):
                cktNode = self.ckt.node(nodeIdx)
                subCkt = self.dDB.subCkt(cktNode.graphIdx)
                cktNode.setOffset(0, 0)
                self.ckt.layout().insertLayout(subCkt.layout(), 0, 0, cktNode.flipVertFlag)
        # Output placement result
        magicalFlow.writeGdsLayout(self.cktIdx, self.dirname + self.ckt.name + '.place.gds', self.dDB, self.tDB)
        self.origin = [0,0]
        if self.debug:
            gdspy.write_gds(self.dirname+self.ckt.name+'.floorplan.gds', [self.tempCell], unit=1.0e-9, precision=1.0e-9)

    def hardcodeConvertPdkLayerToIoLayer(self, pdkLayer):
        #print("WARNING: using hard-coded IO layer conversion")
        if pdkLayer == 31:
            return 1
        elif pdkLayer == 32:
            return 2
        elif pdkLayer == 33:
            return 3
        elif pdkLayer == 34:
            return 4
        elif pdkLayer == 35:
            return 5
        elif pdkLayer == 36:
            return 6
        elif pdkLayer == 37:
            return 7
        else:
            return -1

    def initPowerPins(self):
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if net.isVdd():
                self.initNetPinToPower(netIdx)
            elif net.isVss():
                self.initNetPinToPower(netIdx)

    def initNetPinToPower(self, netIdx, topmet=6):
        net = self.ckt.net(netIdx)
        for pinId in range(net.numPins()):
            pinIdx = net.pinIdx(pinId)
            pin = self.ckt.pin(pinIdx)
            conNode = pin.nodeIdx
            conNet = pin.intNetIdx
            conCkt = self.dDB.subCkt(self.ckt.node(conNode).graphIdx)
            if not pin.valid:
                continue
            if pin.pinType == magicalFlow.PinType.PSUB:
                assert net.isSub, net.name
                if conCkt.implType != magicalFlow.ImplTypePCELL_Cap:
                    continue
            conLayer = conCkt.net(conNet).ioPinMetalLayer(0) 
            ioshape = conCkt.net(conNet).ioPinShape(0)
            if conLayer < topmet:
                met_cell = basic.power_pin_init([ioshape.xLo/1000.0, ioshape.yLo/1000.0], [ioshape.xHi/1000.0, ioshape.yHi/1000.0], conLayer+1, topmet)
                self.addPycell(conCkt.layout(), met_cell)
                conCkt.net(conNet).ioLayer = topmet


    def addPycellIoPinToNet(self, netIdx, offsetX, offsetY, pyCell, isPowerStripe=False):
        polygons = pyCell.get_polygons(True)
        layers = polygons.keys()
        routableShapes = []
        routablePdkLayers = []
        routableDatatype = []
        otherShapes = []
        otherPdkLayers = []
        otherDataType = []
        for layer in layers:
            layerIdx = self.tDB.pdkLayerToDb(layer[0])
            datatype = layer[1]
            polyList = polygons[layer]
            for poly in polyList:
                xLo = int(round(poly[0][0]*200))*5
                yLo = int(round(poly[0][1]*200))*5
                xHi = int(round(poly[2][0]*200))*5
                yHi = int(round(poly[2][1]*200))*5
                if self.hardcodeConvertPdkLayerToIoLayer(layer[0]) != -1:
                    routableShapes.append([ xLo, yLo, xHi, yHi])
                    routablePdkLayers.append(layer[0])
                    routableDatatype.append(datatype)
                    print("metal ", xLo, yLo, xHi, yHi, layer[0])
                else:
                    otherShapes.append([xLo, yLo, xHi, yHi])
                    otherPdkLayers.append(layer[0])
                    otherDataType.append(datatype)
        self.addIoPinToNet(netIdx, offsetX, offsetY, routableShapes, routablePdkLayers, otherShapes, otherPdkLayers, addtocurrentlayout=False, isPowerStripe=isPowerStripe, useDatatype=True, routableDatatype=routableDatatype, otherDataType=otherDataType)


    def addIoPinToNet(self, netIdx, offsetX, offsetY, routableShapes, routablePdkLayers, otherShapes, otherPdkLayers, addtocurrentlayout=False,
            isPowerStripe=False, useDatatype=False, routableDatatype=[], otherDataType=[]):
        routableDbLayers = []
        routableIoLayers = []
        for pdkLayer in routablePdkLayers:
            routableDbLayers.append(self.tDB.pdkLayerToDb(pdkLayer))
            routableIoLayers.append(self.hardcodeConvertPdkLayerToIoLayer(pdkLayer))
        otherDbLayers = []
        for pdkLayer in otherPdkLayers:
            otherDbLayers.append(self.tDB.pdkLayerToDb(pdkLayer))
        net = self.ckt.net(netIdx)

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
        for mIdx in range(len(routableShapes)):
            metal = routableShapes[mIdx]
            mPdkLayer = routablePdkLayers[mIdx]
            mIoLayer = routableIoLayers[mIdx]
            dbLayer = routableDbLayers[mIdx]
            net.addIoPin(metal[0] + offsetX,  metal[1] + offsetY,  metal[2] + offsetX,  metal[3] + offsetY, mIoLayer)
            if isPowerStripe:
                net.markLastIoPowerStripe()
            if addtocurrentlayout:
                rectIdx = self.ckt.layout().insertRect(dbLayer, metal[0] + offsetX,  metal[1] + offsetY,  metal[2] + offsetX,  metal[3] + offsetY)
                if useDatatype:
                    self.ckt.layout().setRectDatatype(dbLayer, rectIdx, routableDatatype[mIdx])
            print("pin shape", metal[0] + offsetX,  metal[1] + offsetY,  metal[2] + offsetX,  metal[3] + offsetY)
            print("layer ", mIoLayer)
            rectIdx = iopinGraph.layout().insertRect(dbLayer,  metal[0],  metal[1],  metal[2],  metal[3])
            if useDatatype:
                iopinGraph.layout().setRectDatatype(dbLayer, rectIdx, routableDatatype[mIdx])
            subiopinNet.addIoPin( metal[0],  metal[1],  metal[2],  metal[3], mIoLayer)
            if isPowerStripe:
                subiopinNet.markLastIoPowerStripe()
        for cIdx in range(len(otherShapes)):
            other = otherShapes[cIdx]
            oPdkLayer = otherPdkLayers[cIdx]
            dbLayer = otherDbLayers[cIdx]
            rectIdx = iopinGraph.layout().insertRect(dbLayer, other[0], other[1], other[2], other[3])
            if useDatatype:
                rectIdx = iopinGraph.layout().setRectDatatype(dbLayer, rectIdx, otherDataType[cIdx])
            if addtocurrentlayout:
                rectIdx = self.ckt.layout().insertRect(dbLayer, other[0] + offsetX,  other[1] + offsetY,  other[2] + offsetX,  other[3] + offsetY)
                if useDatatype:
                    self.ckt.layout().setRectDatatype(dbLayer, rectIdx, otherDataType[cIdx])
        zeroOrigin = [0, 0]
        self.upscaleBBox(self.gridStep, iopinGraph, zeroOrigin)
        iopinNode.graphIdx = iopinGraphIdx
        iopinNode.appendPinIdx(iopinPinIdx)
        iopinPin = self.ckt.pin(iopinPinIdx)
        iopinPin.intNetIdx = 0
        iopinPin.nodeIdx = iopinNodeIdx
        iopinPin.netIdx = netIdx
    def addPowerStripe(self, boundary):
        # Use 80 nm spacing.
        width = int(boundary.xLen() + 2 * self.gridStep)
        width = width + (self.gridStep - (width % self.gridStep))
        if width / self.gridStep % 2 == 0:
            width += self.gridStep
        height = int(1500)  # Just give a try
        height = height + (self.gridStep - (height % self.gridStep))  
        if height / self.gridStep % 2 == 0:
            height += self.gridStep
        fWidth = float(width) / 1000.0
        fHeight = float(height) / 1000.0
        vddOffset = [0.0, 0.0]
        vddOffset[0] = ( self.origin[0] - self.gridStep ) / 1000.0
        vddOffset[1] = ( float(boundary.yHi +  4 * self.gridStep)) / 1000.0
        vssOffset = [0.0, 0.0]
        vssOffset[0] = ( self.origin[0] - self.gridStep    ) / 1000.0
        vssOffset[1] = ( float(self.origin[1] - 36 * self.gridStep + self.gridStep / 2)) / 1000.0 
        print("width, ", fWidth, "height", fHeight, "vdd offset", vddOffset[0], vddOffset[1])
        print("width, ", fWidth, "height", fHeight, "vss offset", vssOffset[0], vssOffset[1])
        vddStripe = basic.power_strip(fWidth, fHeight, vddOffset, lay=[6])
        print("generated vdd")
        vssStripe = basic.power_strip(fWidth, fHeight, vssOffset, lay=[6])
        print("generated vss")
        for netIdx in range(self.ckt.numNets()):
            net = self.ckt.net(netIdx)
            if net.isVdd():
                print("\n\n\n\n add vdd ")
                self.addPycellIoPinToNet(netIdx, 0, 0, vddStripe, isPowerStripe=True)
            elif net.isVss():
                print("\n\n\n\n add vss ")
                self.addPycellIoPinToNet(netIdx, 0, 0, vssStripe, isPowerStripe=True)


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
            self.placer.setNetName(dbNetIdx, net.name) # FIXME: the only purpose is to use parse symnet file. Which ideally can be avoided
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
                self.origin[0] = self.placer.xCellLoc(nodeIdx)
            if self.origin[1] > self.placer.yCellLoc(nodeIdx):
                self.origin[1] = self.placer.yCellLoc(nodeIdx)
        if self.useIoPin:
            for netIdx in range(self.ckt.numNets()):
                net = self.ckt.net(netIdx)
                if (net.isIo() and (not net.isPower())):
                    ioPinX = self.placer.iopinX(netIdx)
                    ioPinY = self.placer.iopinY(netIdx)
                    if self.origin[0] > ioPinX:
                        self.origin[0] = ioPinX
                    if  self.origin[1] > ioPinY:
                        self.origin[1] = ioPinY
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
        
