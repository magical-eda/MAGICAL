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
import os
import Router
import Placer
import gdspy
from device_generation.glovar import tsmc40_glovar as glovar

class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.rootCktIdx = self.mDB.topCktIdx()
        # debug mode will output .pin and .gr file
        self.halfMetWid = int(glovar.min_w['M1']*1000)/2
        self.gridStep = int((glovar.min_w['M1'] + glovar.min_w['SP'])*1000)
        self.debug = True
        self.params = self.mDB.params
        self.runtime = 0

    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        if self.rootCktIdx == cktIdx:
            self.isTopLevel = True
        else:
            self.isTopLevel = False
        print("PnR: working on ", self.dDB.subCkt(cktIdx).name)
        self.runPlace(cktIdx, dirname)
        self.checkSmallModule(cktIdx)
        self.runRoute(cktIdx, dirname)
        self.dDB.subCkt(cktIdx).isImpl = True
        print("PnR: finished ", self.dDB.subCkt(cktIdx).name)

    def placeOnly(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        if self.rootCktIdx == cktIdx:
            self.isTopLevel = True
        else:
            self.isTopLevel = False
        print("PnR: working on ", self.dDB.subCkt(cktIdx).name, self.rootCktIdx, self.dDB.rootCktIdx(), cktIdx)
        self.cktIdx = cktIdx
        self.dirname = dirname
        self.runPlace(cktIdx, dirname, False)
        self.checkSmallModule(cktIdx)
        self.dDB.subCkt(cktIdx).isImpl = True
        print("PnR: placement finished ", self.dDB.subCkt(cktIdx).name)

    def placeAndRoute(self):
        self.runPlace(self.cktIdx, self.dirname, True)
        self.checkSmallModule(self.cktIdx)
        self.dDB.subCkt(self.cktIdx).isImpl = True
        self.routeOnly()

    def floorplan(self):
        # determine whether really need floorplan
        ckt = self.dDB.subCkt(self.cktIdx)
        if ckt.implType != magicalFlow.ImplTypeUNSET:
            return
        subCktNotAllLeaf = False
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            if node.isLeaf():
                continue
            subCktIdx = node.graphIdx
            subCkt = self.dDB.subCkt(subCktIdx)
            if subCkt.implType == magicalFlow.ImplTypeUNSET and node.name != "":
                subCktNotAllLeaf = True
                print("HERE WE GO ", subCkt.name, "|", node.name, "|")
                break
        if not subCktNotAllLeaf:
            return

        pro = magicalFlow.TopFloorplanProblem()
        pro.initProblem(self.dDB, self.dDB.subCkt(self.cktIdx), self.dirname)
        ilp = magicalFlow.IlpTopFloorplanProblem(pro)
        ilp.solve()
        result = magicalFlow.TopFloorplanProblemResult()
        ilp.writeOut(result)

    def routeOnly(self):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """

        self.p.updatePlacementResult()
        self.runRoute(self.cktIdx, self.dirname)
        print("PnR: routing finished ", self.dDB.subCkt(self.cktIdx).name)
            
    def runPlace(self, cktIdx, dirname, implRealLayout):
        self.p = Placer.Placer(self.mDB, cktIdx, dirname,self.gridStep, self.halfMetWid)
        #if not self.isTopLevel:
        #    self.p.implRealLayout = False
        #    self.p.run()
        #    self.p.resetPlacer()
        #    #self.p.run()
        #    #self.p.resetPlacer()
        #    #self.p.run()
        #    #self.p.resetPlacer()
        self.p.implRealLayout = implRealLayout
        self.p.run()
        self.runtime += self.p.runtime
        self.symAxis = self.p.symAxis
        self.origin = self.p.origin
        self.subShapeList = self.p.subShapeList
        self.upscaleBBox(self.gridStep, self.dDB.subCkt(cktIdx), self.origin)

    def runRoute(self, cktIdx, dirname):
        print("runtime, ", self.runtime)
        ckt = self.dDB.subCkt(cktIdx)
        self.routerNets = []
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            #if net.isPower() and self.isTopLevel:
            #    continue
            self.routerNets.append(netIdx)
        self.findOrigin(cktIdx)
        router = anaroutePy.AnaroutePy()
        router.setCircuitName(ckt.name)
        placeFile = dirname + ckt.name + '.place.gds'
        if self.debug:
            iopinfile = dirname + ckt.name + ".iopin"
            self.writeiopifile(cktIdx, iopinfile)
        router.parseLef(self.params.lef)
        router.parseTechfile(self.params.techfile)
        router.parseGds(placeFile)
        self.routeParsePin(router, cktIdx, dirname+ckt.name+'.gr')  
        router.setGridStep(2*self.gridStep)
        router.setSymAxisX(2*self.symAxis)
        router.setGridOffsetX(2*(self.origin[0] - self.gridStep * 10))
        router.setGridOffsetY(2*(self.origin[1] - self.gridStep * 10))
        print("routing grid off set", 2*(self.origin[0]), 2*(self.origin[1]))
        #router.parseSymNet(dirname+ckt.name+'.symnet')
        if self.isTopLevel:
            for netIdx in self.routerNets:
                net = ckt.net(netIdx)
                if net.isIo():
                    router.addIOPort(net.name)
        routerPass = router.solve(False)
        router.evaluate()
        if not routerPass:
            print("Routing failed! ckt ", ckt.name)
            assert(routerPass)
        router.writeLayoutGds(placeFile, dirname+ckt.name+'.route.gds', True)
        router.writeDumb(placeFile, dirname+ckt.name+'.ioPin') 
        # Read results to flow
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')
        self.upscaleBBox(self.gridStep, ckt, self.origin)

    def upscaleBBox(self, gridStep, ckt, origin):
        """
        @brief for legalize the boundary box after the routing. The routing wire might change the boundary of placement, so that the bounding box need to be adjust to multiple of grid step
        """
        bBox = ckt.layout().boundary()
        area = self.dbuToUm(bBox.xHi - bBox.xLo)  * self.dbuToUm(bBox.yHi - bBox.yLo)
        spacingX = self.params.blockSpacingFromAreaTableX[0][1]
        for idx in range(len(self.params.blockSpacingFromAreaTableX)):
            if area > self.params.blockSpacingFromAreaTableX[idx][0]:
                spacingX = self.params.blockSpacingFromAreaTableX[idx][1]
        spacingY = self.params.blockSpacingFromAreaTableY[0][1]
        for idx in range(len(self.params.blockSpacingFromAreaTableY)):
            if area > self.params.blockSpacingFromAreaTableY[idx][0]:
                spacingY = self.params.blockSpacingFromAreaTableY[idx][1]
        spacingX = self.umToDbu(spacingX)
        spacingY = self.umToDbu(spacingY)
        xLoLen = origin[0] - bBox.xLo + spacingX
        yLoLen = origin[1] - bBox.yLo + spacingY
        xHiLen = bBox.xHi - origin[0] + spacingX
        yHiLen = bBox.yHi - origin[1] + spacingY
        xLoLen += gridStep - (xLoLen % gridStep)
        xHiLen += gridStep - (xHiLen % gridStep)
        yLoLen += gridStep - (yLoLen % gridStep)
        yHiLen += gridStep - (yHiLen % gridStep)
        ckt.layout().setBoundary( origin[0] - xLoLen,
                                  origin[1] - yLoLen,
                                  origin[0] + xHiLen,
                                  origin[1] + yHiLen)

    def iterateNetPinShapes(self, cktIdx, netIdx, callback):
        ckt = self.dDB.subCkt(cktIdx)
        net = ckt.net(netIdx)
        grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)
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
            for iopinidx in range(conCkt.net(conNet).numIoPins()):
                conLayer = conCkt.net(conNet).ioPinMetalLayer(iopinidx) - 1
                ioshape = conCkt.net(conNet).ioPinShape(iopinidx)
                conShape = self.adjustIoShape(ioshape, ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                # GDS and LEF unit mismatch, multiply by 2
                assert conShape[0] <= conShape[2]
                assert conShape[1] <= conShape[3]
                callback(conShape, False)
        if isPsub:
            assert self.cktNeedSub(cktIdx)
            # GDS and LEF unit mismatch, multiply by 2
            for i in range(len(self.subShapeList)):
                if i > 0:
                    continue
                assert self.subShapeList[i][0] <= self.subShapeList[i][2]
                assert self.subShapeList[i][1] <= self.subShapeList[i][3]
                callback(self.subShapeList[0], True)
    def findOrigin(self, cktIdx):
        def updateCallback(shape, isGuardRing):
            if isGuardRing:
                self.updateOriginGuardRing(shape)
            else:
                self.updateOriginPin(shape)
        for netIdx in self.routerNets:
            self.iterateNetPinShapes(cktIdx, netIdx, updateCallback)
        self.origin[0] = int(round(self.origin[0]))
        self.origin[1] = int(round(self.origin[1]))
    def writeiopifile(self, cktIdx, fileName):
        """
        @brief this function write out the .iopin file for router. Primaily for debugging
        """
        ckt = self.dDB.subCkt(cktIdx)
        with open(fileName, 'w') as of:
            for netIdx in self.routerNets:
                net = ckt.net(netIdx)
                if net.isIo():
                    of.write("%s\n"% net.name)
    def routeParsePin(self, router, cktIdx, fileName):
        router.init()
        ckt = self.dDB.subCkt(cktIdx)
        pinName = dict()
        pinNameIdx = 0
        if self.debug:
            outFile = open(fileName, 'w')
            outFile.write('gridStep %d\n' % (self.gridStep))
            outFile.write('Offset %d %d\n' % (self.origin[0],self.origin[1]))
            outFile.write('symAxis %d\n' % (self.symAxis))
            specFile = open(fileName + '.spec', 'w')
        for netIdx in self.routerNets:
            net = ckt.net(netIdx)
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)
            pinName[netIdx] = dict()
            netIsPower = 0
            if net.isPower() and not self.isSmallModule:
                netIsPower = 1
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
                pinName[netIdx][pinId] = pinNameIdx
                iopinshapeIsPowerStripe = 0
                if conCkt.net(conNet).isIoPowerStripe(0) and not self.isSmallModule:
                    if conCkt.net(conNet).isIo():
                        continue # do not give router lower level power stripe pin
                    iopinshapeIsPowerStripe = 1
                print("addPin", str(pinNameIdx), net.isPower(), iopinshapeIsPowerStripe)
                router.addPin(str(pinNameIdx), net.isPower(), iopinshapeIsPowerStripe)
                # Router starts as 0 with M
                for iopinidx in range(conCkt.net(conNet).numIoPins()):
                    conLayer = conCkt.net(conNet).ioPinMetalLayer(iopinidx) - 1
                    ioshape = conCkt.net(conNet).ioPinShape(iopinidx)
                    conShape = self.adjustIoShape(ioshape, ckt.node(conNode).offset(), conCkt.layout().boundary(), ckt.node(conNode).flipVertFlag)
                    # GDS and LEF unit mismatch, multiply by 2
                    assert conShape[0] <= conShape[2]
                    assert conShape[1] <= conShape[3]
                    router.addShape2Pin(pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                    print("addShape2Pin",pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                    if self.debug:
                        string = "%s %s %d %d %d %d %d %d %d\n" % (str(net.name), str(pinNameIdx), conLayer+1, conShape[0], conShape[1], conShape[2], conShape[3], netIsPower, iopinshapeIsPowerStripe)
                        outFile.write(string)
                pinNameIdx += 1
            if isPsub:
                assert self.cktNeedSub(cktIdx)
                pinName[netIdx]['sub'] = pinNameIdx
                router.addPin(str(pinNameIdx), net.isPower(), False)
                print("addPin sub", str(pinNameIdx), net.isPower(), False)
                #router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                for i in range(len(self.subShapeList)):
                    if i > 0:
                        continue
                    assert self.subShapeList[i][0] <= self.subShapeList[i][2]
                    assert self.subShapeList[i][1] <= self.subShapeList[i][3]
                    router.addShape2Pin(pinNameIdx, self.params.psubLayer - 1, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                    print("addShape2Pin psub shape",  self.params.psubLayer - 1, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                    if self.debug:
                        string = "%s %s %d %d %d %d %d %d %d\n" % (net.name, str(pinNameIdx),  self.params.psubLayer, self.subShapeList[i][0], self.subShapeList[i][1], self.subShapeList[i][2], self.subShapeList[i][3], 1, 0)
                        outFile.write(string)   
                pinNameIdx += 1
        if self.debug:
            string = "NET_SPEC:\n" 
            specFile.write(string)   
        for netIdx in self.routerNets: 
            net = ckt.net(netIdx)  
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)    
            width, cuts, rows, cols = self.determineNetWidthVia(cktIdx, netIdx)
            width = self.dbuToRouterDbu(width)
            routerNetIdx = router.addNet(net.name, width, cuts, net.isPower() and not self.isSmallModule, rows, cols)  
            print("addNet netname", net.name, "width", width, "cuts", cuts, "isPower", net.isPower() and not self.isSmallModule, "rows", rows, "cols", cols)
            if self.debug:
                string = "%s %d %d %d %d %d\n" % (net.name, width, cuts, net.isPower() and not self.isSmallModule, rows, cols)
                specFile.write(string)   
            for pinId in range(net.numPins()):
                if pinId in pinName[netIdx]:
                    print("addPin2Net ", pinName[netIdx][pinId], routerNetIdx)
                    router.addPin2Net(pinName[netIdx][pinId], routerNetIdx)
            if isPsub:
                print("addPin2Net pubs ver", pinName[netIdx]['sub'], routerNetIdx)
                router.addPin2Net(pinName[netIdx]['sub'], netIdx)                

    def updateOriginPin(self, shape):
        xCenter = (shape[0] + shape[2]) / 2
        yCenter = (shape[1] + shape[3]) / 2
        if self.origin[0] > xCenter:
            self.origin[0] = xCenter
        if self.origin[1] > yCenter:  
            self.origin[1] = yCenter
            
    def updateOriginGuardRing(self, shape):
        if self.origin[0] > shape[0] + self.halfMetWid:
            self.origin[0] = shape[0] + self.halfMetWid
        if self.origin[1] > shape[1] + self.halfMetWid:  
            self.origin[1] = shape[1] + self.halfMetWid
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
    
    def checkSmallModule(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
        bbox = ckt.layout().boundary()
        xlen = bbox.xLen()
        ylen = bbox.yLen()
        area = xlen * ylen
        if area < self.params.smallModuleAreaThreshold * 1000 * 1000:
            self.isSmallModule = True
        else:
            self.isSmallModule = False
    def determineNetWidthVia(self, cktIdx, netIdx):
        net = self.dDB.subCkt(cktIdx).net(netIdx)
        wTable = self.params.signalAnalogWireWidthTable
        vTable = self.params.signalAnalogViaCutsTable
        if net.isPower():
            wTable = self.params.powerWireWidthTable
            vTable = self.params.powerViaCutsTable
            if self.isSmallModule:
                wTable = self.params.signalAnalogWireWidthTable # if the module is small. no need to use conservative power table
                vTable = self.params.signalAnalogViaCutsTable
            if (self.dDB.subCkt(cktIdx).name == "DIGITAL_TOP_flat"):
                wTable = self.params.dpowerWireWidthTable
        if net.isDigital():
            wTable = self.params.signalDigitalWireWidthTable
            vTable = self.params.signalDigitalViaCutsTable
        length = self.calcNetLength(cktIdx, netIdx)
        width = wTable[0][1]
        for idx in range(len(wTable)):
            if length > wTable[idx][0]:
                width = wTable[idx][1]
        viaType = vTable[0]
        for idx in range(len(vTable)):
            if length > vTable[idx][0]:
                viaType = vTable[idx]
        print("setwidth", self.dDB.subCkt(cktIdx).name, net.name, width)
        print("setvia", viaType[1], viaType[2], viaType[3])
        return (self.umToDbu(width), viaType[1], viaType[2], viaType[3])
    def calcNetLength(self, cktIdx, netIdx):
        """
        @brief calculate the hpwl. return in um
        """
        bbox = [1e20, 1e20, -1e20, -1e20]
        def unionBBox(shape, dontcare):
            bbox[0] = min(bbox[0], shape[0])
            bbox[1] = min(bbox[1], shape[1])
            bbox[2] = max(bbox[2], shape[2])
            bbox[3] = max(bbox[3], shape[3])
        self.iterateNetPinShapes(cktIdx, netIdx, unionBBox)
        wl = max(bbox[2] - bbox[0], bbox[3] - bbox[1])
        if self.dbuToUm(wl) > 100:
            print("longwire", self.dDB.subCkt(cktIdx).name, self.dDB.subCkt(cktIdx).net(netIdx).name, self.dbuToUm(wl))
        return self.dbuToUm(wl)

    def umToDbu(self, um):
        return int(float(self.tDB.units().dbu) * um)
    def dbuToUm(self, dbu):
        return float(dbu) / self.tDB.units().dbu
    def dbuToRouterDbu(self, dbu): #FIXME
        return dbu * 2
