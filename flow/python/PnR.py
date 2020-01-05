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
import device_generation.basic as basic
import Router
import gdspy

class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.dDB.findRootCkt()
        self.rootCktIdx = self.dDB.rootCktIdx()
        # debug mode will output .pin and .gr file
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
        placer.readTechSimpleFile('/inputs/techfile.simple')
        self.placeParsePin(placer, ckt, dirname+ckt.name+'.pin')
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
            x_offset = int(round(x_offset/5.0))*5
            y_offset = int(round(y_offset/5.0))*5
            cktNode.setOffset(x_offset, y_offset)
            ckt.layout().insertLayout(subCkt.layout(), x_offset, y_offset, cktNode.flipVertFlag)
            print cktNode.name, placer.cellName(nodeIdx), x_offset, y_offset, "PLACEMENT"
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

    def runRoute(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        router = anaroutePy.AnaroutePy()
        placeFile = dirname + ckt.name + '.place.gds'
        router.parseLef('/inputs/tcbn40lpbwp_10lm7X2ZRDL.lef')
        router.parseTechfile('/inputs/techfile')
        router.parseGds(placeFile)
        self.routeParsePin(router, cktIdx, dirname+ckt.name+'.gr')        
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
                    device_type = self.dDB.subCkt(ckt.node(nodeIdx).graphIdx).implType
                    if device_type == magicalFlow.ImplTypePCELL_Nch:
                        assert pin.intNetIdx == 3, "placeConnection, sub not NMOS BODY pin"
                    elif device_type == magicalFlow.ImplTypePCELL_Res:
                        assert pin.intNetIdx == 2, "placeConnection, sub not RES BODY pin"
                    else:
                        assert False, "placeConnection, sub not NMOS or RES device"

    def placeParsePin(self, placer, ckt, fileName):
        """
        @brief parse pin and cell for placer
        """
        if self.debug:
            outFile = open(fileName, 'w') #
        for nodeIdx in range(ckt.numNodes()):
            cellIdx = placer.allocateCell()
            assert nodeIdx == cellIdx, "placeParsePin, nodeIdx != ckdIdx"
            node = ckt.node(nodeIdx)
            placer.setCellName(nodeIdx, node.name)
            subCkt = self.dDB.subCkt(node.graphIdx)
            if self.debug:
                outFile.write(placer.cellName(nodeIdx)+'\n') #
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
                string = ' M' + str(layer) + '\t\t((' + str(shape.xLo/1000.0) + ' ' + str(shape.yLo/1000.0) + ') (' + str(shape.xHi/1000.0) + ' ' + str(shape.yHi/1000.0) + '))\n'
                if self.debug:
                    outFile.write(net.name + '\t1\n')
                    outFile.write(string)

    def placeParseBoundary(self, placer, ckt):
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(cktNode.graphIdx)
            bBox = subCkt.layout().boundary()
            placer.addCellShape(nodeIdx, 0, bBox.xLo, bBox.yLo, bBox.xHi, bBox.yHi)
            # Strange debug
            #cell = gdspy.Cell(cktNode.name, True)
            #bound_shape = gdspy.Rectangle((bBox.xLo/1000.0, bBox.yLo/1000.0), (bBox.xHi/1000.0, bBox.yHi/1000.0), 3)
            #cell.add(bound_shape)
            #gdspy.write_gds('/home/unga/jayliu/projects/develop/magical/magical/execution/tsmc40/results/CTDSM_TOP_hao/debug/gds/'+cktNode.name+'.gds',[cell],unit=1.0e-6,precision=1.0e-9)
            
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
        self.subShapeList.append([xlo,ylo,xhi,ylo+w])
        self.subShapeList.append([xlo,ylo,xlo+w,yhi])
        self.subShapeList.append([xhi-w,ylo,xhi,yhi])
        self.subShapeList.append([xlo,yhi-w,xhi,yhi])
        
    def routeParsePin(self, router, cktIdx, fileName):
        ckt = self.dDB.subCkt(cktIdx)
        pinNameIdx = 0
        if self.debug:
            outFile = open(fileName, 'w')
            outFile.write('num net '+ str(ckt.numNets()) + '\n')
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            router.addNet(net.name)
            grPinCount, isPsub = self.netPinCount(ckt, net)
            if self.debug:
                outFile.write(net.name + ' ' + str(netIdx) + ' ' + str(grPinCount) + ' 1\n')
            for pinId in range(net.numPins()):
                pinIdx = net.pinIdx(pinId)
                pin = ckt.pin(pinIdx)
                conNode = pin.nodeIdx
                conNet = pin.intNetIdx
                conCkt = self.dDB.subCkt(ckt.node(conNode).graphIdx)
                if pin.pinType == magicalFlow.PinType.PSUB:
                    assert net.isSub, net.name
                    if conCkt.implType != magicalFlow.ImplTypePCELL_Cap:
                        continue
                # Router starts as 0 with M1
                conLayer = conCkt.net(conNet).ioLayer - 1
                conShape = self.adjustIoShape(conCkt.net(conNet).ioShape(), ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                router.addPin(str(pinNameIdx))
                router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                router.addShape2Pin(pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                pinNameIdx += 1
                if self.debug:
                    string = str(conLayer+1) + ' ' + self.rectToPoly(conShape)
                    outFile.write(string)
            if isPsub:
                print net.name
                assert self.cktNeedSub(cktIdx)
                router.addPin(str(pinNameIdx))
                router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                for i in range(len(self.subShapeList)):
                    router.addShape2Pin(pinNameIdx, 0, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                pinNameIdx += 1
                if self.debug:
                    string = '1 ' + self.rectToPoly(self.subShapeList[0])
                    outFile.write(string) 
            
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
