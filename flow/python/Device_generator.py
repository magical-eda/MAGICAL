#
# @file Device_generator.py
# @date 07/07/2019
# @brief The class for generating the layout for pcell devices
#

import sys, os
from device_generation.Mosfet import Mosfet
from device_generation.Capacitor import Capacitor
from device_generation.Resistor import Resistor
import device_generation.basic as basic
import gdspy
import magicalFlow

class Device_generator(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB

    def setGDS(self, outfile):
        """
        @brief Set output GDSII file
        Should be removed after writing to Cpp DB
        """
        self.outGDS = outfile

    def setPinBB(self, outfile):
        """
        @brief Set output Pin and BB file
        """
        self.outPinBB = outfile

    def writeOut(self):
        """
        @brief write GDSII, Pin, Bin for file based version
        Need to setGDS() and setPinBB
        Should be removed in later versions
        """
        gdspy.write_gds(self.outGDS, [self.cell], unit=1.0e-6, precision=1.0e-9)
        #of = open(self.outPinBB, 'w')
        #BB = basic.BB(self.cell)
        #of.write("%d %d %d %d\n" % (BB[0], BB[1], BB[2], BB[3]))
        #for pin in self.cell.pin():
        #    of.write(pin.normalize())
        #of.close

    def writeDB(self, cktIdx, flipCell):
        """
        @brief write to GdsData. Current version GDS is file based
        Need to setGDS()
        Should write to layoutDB in future.
        """
        gdspy.write_gds(self.outGDS, [self.cell], unit=1.0e-6, precision=1.0e-9)
        ckt = self.dDB.subCkt(cktIdx)
        gdsData = ckt.GdsData()
        BB = basic.basic.BB(self.cell, flipCell)
        #bound = self.cell.cell.get_bounding_box()
        gdsData.setBBox(int(BB[0]), int(BB[1]), int(BB[2]), int(BB[3]))
        ckt.layout().setBoundary(int(BB[0]), int(BB[1]), int(BB[2]), int(BB[3]))
        gdsData.gdsFile = self.outGDS
        # Match pin name, current implementation is integer, bulk need to be ommited for res/cap/mos
        # Example, 0:drain, 1:gate, etc...
        nets = dict()
        for idx in range(ckt.numNets()):
            nets[int(ckt.net(idx).name)] = idx
        net_name = 0
        for pin in self.cell.pin():
            if net_name not in nets:
                net_name += 1
                continue
            shape = pin.normalize_shape()
            if shape[1] > shape[3]:
                ckt.net(nets[net_name]).setIoShape(shape[3], shape[2], shape[1], shape[4])
            else:
                ckt.net(nets[net_name]).setIoShape(shape[1], shape[2], shape[3], shape[4])
                assert shape[1] < shape[3], "Device_generator, xLo = xHi"
            assert shape[2] < shape[4], "Device_generator, yLo > yHi"
            ckt.net(nets[net_name]).ioLayer = shape[0]
            net_name += 1

 
    @staticmethod
    def get_attr(string):
        """
        @brief return attr list based on string
        """
        #string  = string.encode('ascii', 'ignore')
        attr = string.split('_')
        if '25ud18' in attr:
            attr.remove('25ud18')
            attr.append('25')
            attr.append('ud')
        if 'rppolywo' in attr:
            attr.append('wo')
        return attr[1:]

    @staticmethod
    def norm_val(inval, o_unit=1e-6, i_unit=1e-12):
        """
        @brief normalize value for device generation, output could be float
        Dirty Implementation
        """
        outval = (inval*i_unit*1.0)/o_unit
        return outval

    def generateDevice(self, cktIdx, dirname, flipCell=False, writeFile=None):
        ckt = self.dDB.subCkt(cktIdx)
        cirname = ckt.name
        implIdx = ckt.implIdx
        implType = ckt.implType
        phyDB = self.dDB.phyPropDB()
        #print "Generating Device " + cirname
        if implType == magicalFlow.ImplTypePCELL_Nch:
            nch = phyDB.nch(implIdx)
            pinConType = nch.pinConType
            bulkCon = []
            for i in range(nch.numBulkCon()):
                bulkCon.append(nch.bulkCon(i))
            self.cell = Mosfet(True, ckt.name, self.norm_val(nch.width), self.norm_val(nch.length), nch.numFingers, self.get_attr(nch.attr), pinConType=pinConType, bulkCon=bulkCon)
        elif implType == magicalFlow.ImplTypePCELL_Pch:
            pch = phyDB.pch(implIdx)
            pinConType = pch.pinConType
            bulkCon = []
            for i in range(pch.numBulkCon()):
                bulkCon.append(pch.bulkCon(i))
            self.cell = Mosfet(False, ckt.name, self.norm_val(pch.width), self.norm_val(pch.length), pch.numFingers, self.get_attr(pch.attr), pinConType=pinConType, bulkCon=bulkCon)
        elif implType == magicalFlow.ImplTypePCELL_Res:
            res = phyDB.resistor(implIdx)
            self.cell = Resistor(res.series, ckt.name, self.norm_val(res.wr), self.norm_val(res.lr), res.segNum, self.norm_val(res.segSpace), self.get_attr(res.attr))
        elif implType == magicalFlow.ImplTypePCELL_Cap:
            cap = phyDB.capacitor(implIdx)
            self.cell = Capacitor(ckt.name, self.norm_val(cap.w), self.norm_val(cap.spacing), cap.numFingers, self.norm_val(cap.lr), cap.stm, cap.spm, self.get_attr(cap.attr), self.norm_val(cap.ftip))
        else:
            raise Exception("Unsupported device")
            return False
        if flipCell:
            self.cell.flip_vert()
        self.setGDS(dirname+cirname+'.gds')
        #self.setPinBB(dirname+cirname+'.pin')
        self.writeOut()
        self.writeDB(cktIdx, flipCell)
        #self.readGDS(cktIdx, dirname)
        return True

    def readGDS(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        ckt.setTechDB(self.tDB)
        cirname = ckt.name
        fileName = dirname + cirname + '.gds'
        ckt.parseGDS(fileName)
        ckt.layout().setBoundary(ckt.gdsData().bbox().xLo, ckt.gdsData().bbox().yLo, ckt.gdsData().bbox().xHi, ckt.gdsData().bbox().yHi)
        #ckt.layout().insertRect(100,ckt.gdsData().bbox().xLo, ckt.gdsData().bbox().yLo, ckt.gdsData().bbox().xHi, ckt.gdsData().bbox().yHi) 
