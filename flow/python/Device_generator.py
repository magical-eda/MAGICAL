##
# @file Device_generator.py
# @date 07/07/2019
# @brief The class for generating the layout for pcell devices
#

import sys, os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))+'/constraint_generation/python/')
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
        of = open(self.outPinBB, 'w')
        BB = basic.BB(self.cell)
        of.write("%d %d %d %d\n" % (BB[0], BB[1], BB[2], BB[3]))
        for pin in self.cell.pin():
            of.write(pin.normalize())
        of.close

    def writeDB(self, cktIdx):
        """
        @brief write to GdsData. Current version GDS is file based
        Need to setGDS()
        Should write to layoutDB in future.
        """
        gdspy.write_gds(self.outGDS, [self.cell], unit=1.0e-6, precision=1.0e-9)
        ckt = self.dDB.subCkt(cktIdx)
        gdsData = ckt.GdsData()
        BB = basic.BB(self.cell)
        gdsData.setBBox(int(BB[0]), int(BB[1]), int(BB[2]), int(BB[3]))
        gdsData.gdsFile = self.outGDS
        # Match pin name, current implementation is integer, bulk need to be ommited for res/cap/mos
        # Example, 0:drain, 1:gate, etc...
        net_name = 0
        for pin in self.cell.pin():
            shape = pin.normalize_shape()
            ckt.net(net_name).setIoShape(shape[1], shape[2], shape[3], shape[4])
            ckt.net(net_name).ioLayer = shape[0]
            net_name += 1

 
    @staticmethod
    def get_attr(string):
        """
        @brief return attr list based on string
        """
        string  = string.encode('ascii', 'ignore')
        attr = string.split('_')
        return attr[1:]

    @staticmethod
    def norm_val(inval, o_unit=1e-6, i_unit=1e-12):
        """
        @brief normalize value for device generation, output could be float
        Dirty Implementation
        """
        outval = (inval*i_unit*1.0)/o_unit
        return outval

    def generateDevice(self, cktIdx, dirname):
        ckt = self.dDB.subCkt(cktIdx)
        cirname = ckt.name
        implIdx = ckt.implIdx
        implType = ckt.implType
        phyDB = self.dDB.phyPropDB()
        if implType == magicalFlow.ImplTypePCELL_Nch:
            nch = phyDB.nch(implIdx)
            self.cell = Mosfet(True, ckt.name, self.norm_val(nch.width), self.norm_val(nch.length), nch.numFingers, self.get_attr(nch.attr))
        elif implType == magicalFlow.ImplTypePCELL_Pch:
            pch = phyDB.pch(implIdx)
            self.cell = Mosfet(False, ckt.name, self.norm_val(pch.width), self.norm_val(pch.length), pch.numFingers, self.get_attr(pch.attr))
        elif implType == magicalFlow.ImplTypePCELL_Res:
            res = phyDB.resistor(implIdx)
            self.cell = Resistor(res.series, ckt.name, self.norm_val(res.wr), self.norm_val(res.lr), res.segNum, self.norm_val(res.segSpace), self.get_attr(res.attr))
        elif implType == magicalFlow.ImplTypePCELL_Cap:
            cap = phyDB.capacitor(implIdx)
            self.cell = Capacitor(ckt.name, self.norm_val(cap.w), self.norm_val(cap.spacing), cap.numFingers, self.norm_val(cap.lr), cap.stm, cap.spm, self.get_attr(cap.attr), self.norm_val(cap.ftip))
        else:
            raise Exception("Unsupported device")
            return False
        self.setGDS(dirname+cirname+'.gds')
        self.setPinBB(dirname+cirname+'.pin')
        self.writeOut()
        self.writeDB(cktIdx)
        return True
