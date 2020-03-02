#
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
import gdspy
import device_generation.glovar as glovar

class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.dDB.findRootCkt()
        self.rootCktIdx = self.dDB.rootCktIdx()
        # debug mode will output .pin and .gr file
        self.halfMetWid = int(glovar.min_w['M1']*1000)/2
        self.gridStep = int((glovar.min_w['M1'] + glovar.min_w['SP'])*1000)
        self.debug = True

    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        self.runPlace(cktIdx, dirname)
        self.runRoute(cktIdx, dirname)
        self.dDB.subCkt(cktIdx).isImpl = True
            
    def runPlace(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        cktname = ckt.name
        placer = IdeaPlaceExPy.IdeaPlaceEx()
        placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
        self.placeParsePin(placer, ckt, dirname+ckt.name+'.pin')
        self.placeConnection(placer, ckt, dirname+ckt.name+'.con')
        placer.readSymFile(dirname + cktname + '.sym')
        self.placeParseBoundary(placer, ckt, dirname+ckt.name+'.bound')
        if self.debug:
            gdspy.current_library = gdspy.GdsLibrary()
            tempCell = gdspy.Cell("FLOORPLAN")
        self.symAxis = placer.solve(self.gridStep)
        #placer.alignToGrid(200)
        #  Set Placement origin
        self.setPlaceOrigin(placer, ckt.numNodes())
        self.symAxis = int(self.symAxis - self.origin[0])
        # Write results to flow
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            x_offset = placer.xCellLoc(nodeIdx) - self.origin[0]
            y_offset = placer.yCellLoc(nodeIdx) - self.origin[1]
            cktNode.setOffset(x_offset, y_offset)
            ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
            print cktNode.name, placer.cellName(nodeIdx), x_offset, y_offset, "PLACEMENT"
            if self.debug:
                boundary = subCkt.layout().boundary()
                rect = gdspy.Rectangle((boundary.xLo+x_offset,boundary.yLo+y_offset), (boundary.xHi+x_offset,boundary.yHi+y_offset))
                text = gdspy.Text(cktNode.name,50,((boundary.xLo+boundary.xHi)/2+x_offset,(boundary.yLo+boundary.yHi)/2+y_offset),layer=100)
                tempCell.add(rect)
                tempCell.add(text)
        # write guardring using gdspy
        if self.cktNeedSub(cktIdx):
            print "Adding GuardRing to Cell"
            bBox = ckt.layout().boundary()
            # Leave additional 80nm spacing
            grCell, subPin = basic.sub_GR([bBox.xLo/1000.0-0.08, bBox.yLo/1000.0-0.08], [bBox.xHi/1000.0+0.08, bBox.yHi/1000.0+0.08], [-self.halfMetWid/1000.0,-self.halfMetWid/1000.0])
            self.addPycell(ckt.layout(), grCell)
            bBox = ckt.layout().boundary()
            self.subShape(subPin)
        # Output placement result
        magicalFlow.writeGdsLayout(cktIdx, dirname + cktname + '.place.gds', self.dDB, self.tDB)
        self.origin = [0,0]
        if self.debug:
            gdspy.write_gds(dirname+ckt.name+'.floorplan.gds', [tempCell], unit=1.0e-9, precision=1.0e-9)

    def runRoute(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        router = anaroutePy.AnaroutePy()
        placeFile = dirname + ckt.name + '.place.gds'
        router.parseLef('/home/unga/jayliu/projects/inputs/tcbn40lpbwp_10lm7X2ZRDL.lef')
        router.parseTechfile('/home/unga/jayliu/projects/inputs/techfile')
        router.parseGds(placeFile)
        self.routeParsePin(router, cktIdx, dirname+ckt.name+'.gr')  
        router.setGridStep(2*self.gridStep)
        router.setSymAxisX(2*self.symAxis)
        router.setGridOffsetX(2*(self.origin[0]))
        router.setGridOffsetY(2*(self.origin[1]))
        #print self.origin[0]+self.halfMetWid, self.origin[1]+self.halfMetWid, "OFFSET"
        router.parseSymNet(dirname+ckt.name+'.symnet')
        if cktIdx == self.rootCktIdx:
            for netIdx in range(ckt.numNets()):
                net = ckt.net(netIdx)
                if net.isIo():
                    print net.name, "added to IO port"
                    router.addIOPort(net.name)
        router.solve()
        router.writeLayoutGds(placeFile, dirname+ckt.name+'.route.gds', True)
        router.writeDumb(placeFile, dirname+ckt.name+'.ioPin') 
        # Read results to flow
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')
        Router.Router(self.mDB).readBackDumbFile(dirname+ckt.name+'.ioPin', cktIdx)

    def placeConnection(self, placer, ckt, fileName):
        """
        @brief add pin connection info to placer
        """
        if self.debug:
            outFile = open(fileName, 'w')
            outFile.write("%d\n" % ckt.numNets())
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            dbNetIdx = placer.allocateNet()
            assert netIdx == dbNetIdx, "placeConnection, netIdx == dbNetIdx"
            if self.debug:
                outFile.write("%d\n" % net.numPins())
            for pinIdx in range(net.numPins()):
                pinidxidx = net.pinIdx(pinIdx)
                pin = ckt.pin(pinidxidx)
                nodeIdx = pin.nodeIdx
                if not pin.valid:
                    if self.debug:
                        outFile.write("-1\n")
                    continue
                if placer.pinIdx(nodeIdx, pin.intNetIdx) < pow(2,32)-1:
                    placer.addPinToNet(placer.pinIdx(nodeIdx, pin.intNetIdx), netIdx)
                    if self.debug:
                        outFile.write("%d\n" % placer.pinIdx(nodeIdx, pin.intNetIdx))
                else:
                    device_type = self.dDB.subCkt(ckt.node(nodeIdx).graphIdx).implType
                    if device_type == magicalFlow.ImplTypePCELL_Nch:
                        assert pin.intNetIdx == 3, "placeConnection, sub not NMOS BODY pin"
                    elif device_type == magicalFlow.ImplTypePCELL_Res:
                        assert pin.intNetIdx == 2, "placeConnection, sub not RES BODY pin"
                    else:
                        assert False, "placeConnection, sub not NMOS or RES device"
                    if self.debug:
                        outFile.write("-1\n")

    def placeParsePin(self, placer, ckt, fileName):
        """
        @brief parse pin and cell for placer
        """
        if self.debug:
            outFile = open(fileName, 'w') #
            outFile.write("%d\n" % ckt.numNodes())
        for nodeIdx in range(ckt.numNodes()):
            cellIdx = placer.allocateCell()
            assert nodeIdx == cellIdx, "placeParsePin, nodeIdx != ckdIdx"
            node = ckt.node(nodeIdx)
            placer.setCellName(nodeIdx, node.name)
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
                pinIdx = placer.allocatePin(nodeIdx)
                assert pinIdx == placer.pinIdx(nodeIdx, netIdx), "placeParsePin, pin insertion error"
                placer.addPinShape(pinIdx, shape.xLo, shape.yLo, shape.xHi, shape.yHi)
                if self.debug:
                    outFile.write("%d %d %d %d\n" % (shape.xLo, shape.yLo, shape.xHi, shape.yHi))

    def placeParseBoundary(self, placer, ckt, fileName):
        if self.debug:
            outFile = open(fileName, 'w') #
            outFile.write("%d\n" % ckt.numNodes())
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            placer.addCellShape(nodeIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
            if self.debug:
                outFile.write("%d %d %d %d\n" % (bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi))
            #print cktNode.name, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi
            #assert ((bBox.xHi - bBox.xLo)/200)  % 2 == 1
            #assert ((bBox.yHi - bBox.yLo)/200)  % 2 == 1
            # Strange debug
            #cell = gdspy.Cell(cktNode.name, True)
            #bound_shape = gdspy.Rectangle((bBox.xLo/1000.0, bBox.yLo/1000.0), (bBox.xHi/1000.0, bBox.yHi/1000.0), 3)
            #cell.add(bound_shape)
            #gdspy.write_gds('/home/unga/jayliu/projects/develop/magical/magical/execution/tsmc40/integration/USC_ADC12/gds/'+cktNode.name+'.gds',[cell],unit=1.0e-6,precision=1.0e-9)
            
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
        self.subShapeList = list()
        shape = subPin.normalize_shape()
        # Only the lower metal for now
        self.subShapeList.append(shape[1:5])
        
    def routeParsePin(self, router, cktIdx, fileName):
        router.init()
        ckt = self.dDB.subCkt(cktIdx)
        pinName = dict()
        pinNameIdx = 0
        if self.debug:
            outFile = open(fileName, 'w')
            outFile.write('gridStep %d\n' % (self.gridStep))
            outFile.write('symAxis %d\n' % (self.symAxis))
            outFile.write('Offset %d %d\n' % (self.origin[0],self.origin[1]))
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)
            pinName[netIdx] = dict()
            #router.addNet(net.name, 200, 1, (isPsub or isNwell))
            #router.addNet(net.name)
            if self.debug:
                pass
                #outFile.write(net.name + ' ' + str(netIdx) + ' ' + str(grPinCount) + ' 1\n')
            for pinId in range(net.numPins()):
                pinIdx = net.pinIdx(pinId)
                pin = ckt.pin(pinIdx)
                conNode = pin.nodeIdx
                conNet = pin.intNetIdx
                conCkt = self.dDB.subCkt(ckt.node(conNode).graphIdx)
                if not pin.valid:
                    continue
                if pin.pinType == magicalFlow.PinType.PSUB:
                    assert net.isSub, net.name
                    if conCkt.implType != magicalFlow.ImplTypePCELL_Cap:
                        continue
                # Router starts as 0 with M1
                conLayer = conCkt.net(conNet).ioLayer - 1
                conShape = self.adjustIoShape(conCkt.net(conNet).ioShape(), ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                pinName[netIdx][pinId] = pinNameIdx
                router.addPin(str(pinNameIdx))
                #router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                assert conShape[0] <= conShape[2]
                assert conShape[1] <= conShape[3]
                #print pinName[netIdx][pinId], conShape[0], conShape[1]
                self.updateOrigin(conShape)
                router.addShape2Pin(pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                pinNameIdx += 1
                if self.debug:
                    string = "%s %d %d %d %d %d %d %d\n" % (str(net.name), conLayer+1, conShape[0], conShape[1], conShape[2], conShape[3], (conShape[0]+265)/140, (conShape[1]+280)/140)
                    #string = str(conLayer+1) + ' ' + self.rectToPoly(conShape)
                    outFile.write(string)
                print conShape, self.origin
                assert basic.check_legal_coord([conShape[0]/1000.0, conShape[1]/1000.0],[-self.halfMetWid/1000.0,-self.halfMetWid/1000.0]), "Pin Not Legal!"
                #assert basic.check_legal_coord([conShape[2]/1000.0-glovar.min_w['M1'], conShape[3]/1000.0-glovar.min_w['M1']]), "Pin Not Legal!"
            if isPsub:
                assert self.cktNeedSub(cktIdx)
                pinName[netIdx]['sub'] = pinNameIdx
                router.addPin(str(pinNameIdx))
                #router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                for i in range(len(self.subShapeList)):
                    if i > 0:
                        continue
                    assert self.subShapeList[i][0] <= self.subShapeList[i][2]
                    assert self.subShapeList[i][1] <= self.subShapeList[i][3]
                    self.updateOrigin(self.subShapeList[0])
                    router.addShape2Pin(pinNameIdx, 0, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                pinNameIdx += 1
                #assert basic.check_legal_coord([self.subShapeList[0][0]/1000.0, self.subShapeList[0][1]/1000.0],[-self.halfMetWid/1000.0,-self.halfMetWid/1000.0]), "Pin Not Legal!"
                #assert basic.check_legal_coord([self.subShapeList[0][2]/1000.0-glovar.min_w['M1'], self.subShapeList[0][3]/1000.0-glovar.min_w['M1']]), "Pin Not Legal!"
                if self.debug:
                    #string = "%s 1 %d %d %d %d %d %d\n" % (net.name, self.subShapeList[0][0], self.subShapeList[0][1], self.subShapeList[0][2], self.subShapeList[0][3], (self.subShapeList[0][0]+265)/140, (self.subShapeList[0][1]+280)/140)
                    string = "%s 1 %d %d %d %d %d %d\n" % (net.name, self.subShapeList[0][0], self.subShapeList[0][1], self.subShapeList[0][2], self.subShapeList[0][3], (self.subShapeList[0][0]+265)/140, (self.subShapeList[0][1]+280)/140)
                    #string = '1 ' + self.rectToPoly(self.subShapeList[0])
                    outFile.write(string)   
        for netIdx in range(ckt.numNets()): 
            net = ckt.net(netIdx)  
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)    
            router.addNet(net.name, 200, 1, (isPsub or isNwell))  
            #print net.name     
            for pinId in range(net.numPins()):
                if pinId in pinName[netIdx]:
                    #print pinName[netIdx][pinId]
                    router.addPin2Net(pinName[netIdx][pinId], netIdx)
            if isPsub:
                #print "sub"
                router.addPin2Net(pinName[netIdx]['sub'], netIdx)                

    def setPlaceOrigin(self, placer, numNodes):
        self.origin = [placer.xCellLoc(0), placer.yCellLoc(0)]
        for nodeIdx in range(numNodes):
            if self.origin[0] > placer.xCellLoc(nodeIdx):
                self.origin = [placer.xCellLoc(nodeIdx), placer.yCellLoc(nodeIdx)]
            elif self.origin[0] == placer.xCellLoc(nodeIdx) and self.origin[1] > placer.yCellLoc(nodeIdx):
                self.origin = [placer.xCellLoc(nodeIdx), placer.yCellLoc(nodeIdx)]
        # Calibrate to ll corner
        #self.origin = [self.origin[0]-self.halfMetWid , self.origin[1]-self.halfMetWid]

    def updateOrigin(self, shape):
        if self.origin[0] > shape[0]:
            self.origin = [shape[0]+self.halfMetWid, shape[1]+self.halfMetWid]
        elif self.origin[0] == shape[0] and self.origin[1] > shape[1]:  
            self.origin = [shape[0]+self.halfMetWid, shape[1]+self.halfMetWid]
            
    @staticmethod
    def flipPin(xLo, xHi, symAxis_2):
        xLo_s = symAxis_2 - xHi
        xHi_s = symAxis_2 - xLo
        return xLo_s, xHi_s

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
        assert xLo < xHi, "addShape, xLo > xHi"
        assert yLo < yHi, "addShape, yLo > yHi"
        return [xLo, yLo, xHi, yHi]

    @staticmethod
    def rectToPoly(shape):
        newshape = list()
        xlo, ylo, xhi, yhi = shape
        newshape.append(xlo)
        newshape.append(ylo)
        newshape.append(xhi)
        newshape.append(ylo)
        newshape.append(xhi)
        newshape.append(yhi)
        newshape.append(xlo)
        newshape.append(yhi)
        string = ''
        for x in newshape:
            string = string + str(x) + ' '
        return string + '\n'
