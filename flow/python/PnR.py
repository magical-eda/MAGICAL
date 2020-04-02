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
#sys.path.append('/home/local/eda09/keren/projects/magical/constraint_generation/python')
sys.path.append('/home/local/eda10/jayliu/projects/develop/magical/magical/constraint_generation/python')
import device_generation.basic as basic
import Router
import Placer
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
        self.params = self.mDB.params

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
        self.runRoute(cktIdx, dirname)
        self.dDB.subCkt(cktIdx).isImpl = True
        print("PnR: finished ", self.dDB.subCkt(cktIdx).name)
            
    def runPlace(self, cktIdx, dirname):
        p = Placer.Placer(self.mDB, cktIdx, dirname,self.gridStep, self.halfMetWid)
        p.run()
        self.symAxis = p.symAxis
        self.origin = p.origin
        self.subShapeList = p.subShapeList

    def runRoute(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        self.findOrigin(cktIdx)
        self.routerNets = []
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            #if net.isPower() and self.isTopLevel:
            #    continue
            self.routerNets.append(netIdx)
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
        #print self.origin[0]+self.halfMetWid, self.origin[1]+self.halfMetWid, "OFFSET"
        #router.parseSymNet(dirname+ckt.name+'.symnet')
        if self.isTopLevel:
            for netIdx in self.routerNets:
                net = ckt.net(netIdx)
                if net.isIo():
                    print net.name, "added to IO port"
                    router.addIOPort(net.name)
        routerPass = router.solve(False)
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
        xLoLen = origin[0] - bBox.xLo + 200
        yLoLen = origin[1] - bBox.yLo + 200
        xHiLen = bBox.xHi - origin[0] + 200
        yHiLen = bBox.yHi - origin[1] + 200
        xLoLen += gridStep - (xLoLen % gridStep)
        xHiLen += gridStep - (xHiLen % gridStep)
        yLoLen += gridStep - (yLoLen % gridStep)
        yHiLen += gridStep - (yHiLen % gridStep)
        ckt.layout().setBoundary( origin[0] - xLoLen,
                                  origin[1] - yLoLen,
                                  origin[0] + xHiLen,
                                  origin[1] + yHiLen)

    def findOrigin(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
        pinName = dict()
        pinNameIdx = 0
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)
            pinName[netIdx] = dict()
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
                    #print pinName[netIdx][pinId], conShape[0], conShape[1]
                    self.updateOriginPin(conShape)
            if isPsub:
                assert self.cktNeedSub(cktIdx)
                # GDS and LEF unit mismatch, multiply by 2
                for i in range(len(self.subShapeList)):
                    if i > 0:
                        continue
                    assert self.subShapeList[i][0] <= self.subShapeList[i][2]
                    assert self.subShapeList[i][1] <= self.subShapeList[i][3]
                    self.updateOriginGuardRing(self.subShapeList[0])
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
        for netIdx in self.routerNets:
            net = ckt.net(netIdx)
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)
            pinName[netIdx] = dict()
            netIsPower = 0
            if net.isPower():
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
                if conCkt.net(conNet).isIoPowerStripe(0):
                    if conCkt.net(conNet).isIo():
                        continue # do not give router lower level power stripe pin
                    iopinshapeIsPowerStripe = 1
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
                    print("add pin shape", conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                    if self.debug:
                        string = "%s %s %d %d %d %d %d %d %d\n" % (str(net.name), str(pinNameIdx), conLayer+1, conShape[0], conShape[1], conShape[2], conShape[3], netIsPower, iopinshapeIsPowerStripe)
                        outFile.write(string)
                    print conShape, self.origin
                pinNameIdx += 1
            if isPsub:
                assert self.cktNeedSub(cktIdx)
                pinName[netIdx]['sub'] = pinNameIdx
                router.addPin(str(pinNameIdx), net.isPower(), False)
                #router.addPin2Net(pinNameIdx, netIdx)
                # GDS and LEF unit mismatch, multiply by 2
                for i in range(len(self.subShapeList)):
                    if i > 0:
                        continue
                    assert self.subShapeList[i][0] <= self.subShapeList[i][2]
                    assert self.subShapeList[i][1] <= self.subShapeList[i][3]
                    router.addShape2Pin(pinNameIdx, self.params.psubLayer - 1, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                    print("add psub shape",  self.params.psubLayer - 1, self.subShapeList[i][0]*2, self.subShapeList[i][1]*2, self.subShapeList[i][2]*2, self.subShapeList[i][3]*2)
                    if self.debug:
                        string = "%s %s %d %d %d %d %d %d %d\n" % (net.name, str(pinNameIdx),  self.params.psubLayer, self.subShapeList[i][0], self.subShapeList[i][1], self.subShapeList[i][2], self.subShapeList[i][3], 1, 0)
                    #string = '1 ' + self.rectToPoly(self.subShapeList[0])
                    outFile.write(string)   
                pinNameIdx += 1
        for netIdx in self.routerNets: 
            net = ckt.net(netIdx)  
            grPinCount, isPsub, isNwell = self.netPinCount(ckt, net)    
            width = 200
            if net.isPower():
                width = 1000
            routerNetIdx = router.addNet(net.name, width, 1, net.isPower())  
            for pinId in range(net.numPins()):
                if pinId in pinName[netIdx]:
                    router.addPin2Net(pinName[netIdx][pinId], routerNetIdx)
            if isPsub:
                #print "sub"
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
