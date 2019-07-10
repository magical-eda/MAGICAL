##
# @file Device_generator.py
# @date 07/07/2019
# @brief The class for generating the layout for pcell devices
#

import sys
sys.path.append("./../../../constraint_generation/python")
from device_generation.Mosfet import Mosfet
from device_generation.Capacitor import Capacitor
from device_generation.Resistor import Resistor
import device_generation.basic as basic
import gdspy

class Device_generator(object):
    def __init__(self, magicalDB):
        self.dDB = magicalDB
#        self.mDB = magicalDB
#        self.dDB = magicalDB.designDB.db
#        self.tDB = magicalDB.techDB

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
        Should be removed in later versions
        """
        gdspy.write_gds(self.outGDS, [self.cell], unit=1.0e-6, precision=1.0e-9)
        of = open(self.outPinBB, 'a')
        BB = basic.BB(self.cell)
        of.write("%d %d %d %d" % (BB[0][0], BB[0][1], BB[1][0], BB[1][1]))
        for pin in self.cell.pin:
            of.write(pin.normalize())
        of.close
 
    @staticmethod
    def getattr(string):
        """
        @brief return attr list based on string
        """
        string  = string.encode('ascii', 'ignore')
        attr = string.split('_')
        return attr[1:]

    @staticmethod
    def norm_val(inval, o_unit=1e-6, i_unit=1e-9):
        """
        @brief normalize value for device generation, output could be float
        Dirty Implementation
        """
        outval = (inval*i_unit*1.0)/o_unit
        return outval

    def generateDevice(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx)
        implIdx = ckt.implIdx
        implType = ckt.implType
        phyDB = self.dDB.phyPropDB()
        if implType == magicalFlow.ImplTypePCELL_Nch:
            nch = phyDB.nch(implIdx)
            self.cell = Mosfet(True, ckt.name, norm_val(nch.width), norm_val(nch.length), nch.numFingers, get_attr(nch.attr))
        elif implType == magicalFlow.ImplTypePCELL_Pch:
            pch = phyDB.pch(implIdx)
            self.cell = Mosfet(False, ckt.name, norm_val(pch.width), norm_val(pch.length), pch.numFingers, get_attr(pch.attr))
        elif implType == magicalFlow.ImplTypePCELL_Res:
            res = phyDB.resister(implIdx)
            self.cell = Resister(res.series, ckt.name, norm_val(res.wr), norm_val(res.lr), res.segNum, norm_val(res.segSpace), get_attr(res.attr))
        elif implType == magicalFlow.ImplTypePCELL_Cap:
            cap = phyDB.capacitor(implIdx)
            self.cell = Capacitor(ckt.name, norm_val(cap.w), norm_val(cap.spacing), cap.numFingers, norm_val(cap.lr), cap.stm, cap.spm, get_attr(cap.attr), norm_val(cap.ftip))
        else:
            raise Exception("Unsupported device")
            return False
        self.writeOut()
        return True
