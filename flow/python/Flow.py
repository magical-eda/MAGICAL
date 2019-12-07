##
# @file Flow.py
# @author Keren Zhu
# @date 07/07/2019
# @brief The flow
#

import magicalFlow
import Device_generator
import Constraint
import PnR
import StdCell
import subprocess

cell_cnt = 0

class Flow(object):
    def __init__(self, db):
        self.mDB = db # MagicalDB
        self.dDB = db.designDB.db
        self.constraint = Constraint.Constraint(self.mDB)

    def run(self):
        """
        @brief the main function to run the flow
        @return if successful
        """
        self.resultName = self.mDB.params.resultDir
        topCktIdx = self.mDB.topCktIdx() # The index of the topckt
        self.implCktLayout(topCktIdx)
        return True

    def setup(self, cktIdx):
    # This setup is usded to flip the gds layout cell due to symmetry constraint if needed
    # The devices pins are all flipped and correct
    # The subcircuit cells pins are not flipped and the only the flipVertFlag is set
    # The pins will be flipped in python according to the flag when generating GR file
        ckt = self.dDB.subCkt(cktIdx) 
        for nodeIdx in range(ckt.numNodes()):
            flipCell = False
            cktNode = ckt.node(nodeIdx)
            # Flip cell if is in the "right" half device of symmetry
            if cktNode.name in self.symDict.values():
                flipCell = True
            if cktNode.isLeaf():
                continue
            subCktIdx = self.dDB.subCkt(cktIdx).node(nodeIdx).graphIdx
            subCktName = self.dDB.subCkt(subCktIdx).name
            if magicalFlow.isImplTypeDevice(self.dDB.subCkt(subCktIdx).implType):
                # Lazy implementation, simply regenerate device cell with flipCell flag set
                if flipCell:
                    Device_generator.Device_generator(self.mDB).generateDevice(subCktIdx, self.resultName+'/gds/', True)
                continue
            cmd = "/home/unga/jayliu/projects/develop/magical/magical/install/python/bin/renameGds " \
            + self.resultName + subCktName + ".route.gds " \
            + self.dDB.subCkt(cktIdx).node(nodeIdx).name \
            + " ./gds/" + self.resultName + self.dDB.subCkt(cktIdx).node(nodeIdx).name + ".gds"
            if flipCell:
                symAxis = (self.dDB.subCkt(subCktIdx).gdsData().bbox().xLo + self.dDB.subCkt(subCktIdx).gdsData().bbox().xHi)/2
                cmd = cmd + " " + str(symAxis)
                cktNode.flipVertFlag = True
            subprocess.call(cmd, shell=True)

    def implCktLayout(self, cktIdx):
        """
        @brief implement the circuit layout
        """
        dDB = self.mDB.designDB.db #c++ database
        ckt = dDB.subCkt(cktIdx) #magicalFlow.CktGraph
        # If the ckt is a device
        global cell_cnt
        if magicalFlow.isImplTypeDevice(ckt.implType):
            Device_generator.Device_generator(self.mDB).generateDevice(cktIdx, self.resultName+'/gds/')
            cell_cnt += 1
            return
        # If the ckt is a standard cell
        # This version only support DFCNQD2BWP and NR2D8BWP, hard-encoded
        # TODO: This should be parsed from the json file
        if ckt.name in ['DFCNQD2BWP', 'NR2D8BWP_LVT', 'INVD4BWP_LVT', 'DFCND4BWP_LVT']:#, 'BUFFD4BWP_LVT']:
            StdCell.StdCell(self.mDB).setup(cktIdx, self.resultName)
            return
        # If the ckt is actually a circuit instead of a device
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx) # magicalFlow.CktNode
            if cktNode.isLeaf(): # Do not go deeper for leaf node
                continue
            subCkt = dDB.subCkt(cktNode.graphIdx)
            if (subCkt.isImpl): # Do not duplicately implement the layout
                continue
            self.implCktLayout(cktNode.graphIdx) # Recursively implement all the children
        # After all the children being implemented. P&R at this circuit
        print "Implementing circuit", ckt.name
        self.symDict = self.constraint.genConstraint(cktIdx, self.resultName)
        self.setup(cktIdx)
        PnR.PnR(self.mDB).implLayout(cktIdx, self.resultName)
        #print cell_cnt, "COUNT"
