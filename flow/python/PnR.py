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

    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        self.runPlace(cktIdx, dirname)
        self.runRoute(cktIdx, dirname)
        self.dDB.subCkt(cktIdx).isImpl = True
            
    def runPlace(self, cktIdx, dirname):
        p = Placer.Placer(self.mDB, cktIdx, dirname,self.gridStep, self.halfMetWid)
        p.run()
        self.symAxis = p.symAxis
        self.origin = p.origin
        self.subShapeList = p.subShapeList

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
        print("routing grid off set", 2*(self.origin[0]), 2*(self.origin[1]))
        #print self.origin[0]+self.halfMetWid, self.origin[1]+self.halfMetWid, "OFFSET"
        #router.parseSymNet(dirname+ckt.name+'.symnet')
        if cktIdx == self.rootCktIdx:
            for netIdx in range(ckt.numNets()):
                net = ckt.net(netIdx)
                if net.isIo():
                    print net.name, "added to IO port"
                    router.addIOPort(net.name)
        router.solve(False)
        router.writeLayoutGds(placeFile, dirname+ckt.name+'.route.gds', True)
        #router.writeDumb(placeFile, dirname+ckt.name+'.ioPin') 
        # Read results to flow
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirname+ckt.name+'.route.gds')
        #Router.Router(self.mDB).readBackDumbFile(dirname+ckt.name+'.ioPin', cktIdx)

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
                self.updateOriginPin(conShape)
                router.addShape2Pin(pinNameIdx, conLayer, conShape[0]*2, conShape[1]*2, conShape[2]*2, conShape[3]*2)
                pinNameIdx += 1
                if self.debug:
                    string = "%s %d %d %d %d %d %d %d\n" % (str(net.name), conLayer+1, conShape[0], conShape[1], conShape[2], conShape[3], (conShape[0]+265)/140, (conShape[1]+280)/140)
                    #string = str(conLayer+1) + ' ' + self.rectToPoly(conShape)
                    outFile.write(string)
                print conShape, self.origin
                #assert basic.check_legal_coord([conShape[0]/1000.0, conShape[1]/1000.0],[-self.halfMetWid/1000.0,-self.halfMetWid/1000.0]), "Pin Not Legal!"
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
                    self.updateOriginGuardRing(self.subShapeList[0])
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

    def updateOriginPin(self, shape):
        if self.origin[0] > shape[0] + self.gridStep / 2:
            self.origin[0] = shape[0] + self.gridStep / 2
        if self.origin[1] > shape[1] + self.gridStep / 2:  
            self.origin[1] = shape[1] + self.gridStep / 2
            
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
