##
# @file Flow.py
# @author Keren Zhu
# @date 07/07/2019
# @brief The flow
#

import magicalFlow
import Device_generator
import PnR
import subprocess

class Flow(object):
    def __init__(self, db):
        self.mDB = db # MagicalDB
        self.dDB = db.designDB.db

    def run(self):
        """
        @brief the main function to run the flow
        @return if successful
        """
        self.resultName = self.mDB.params.resultDir
        topCktIdx = self.mDB.topCktIdx() # The index of the topckt
        self.implCktLayout(topCktIdx)
        return True

    def stupidSetup(self, cktIdx):
        ckt = self.dDB.subCkt(cktIdx) 
        for nodeIdx in range(ckt.numNodes()):
            cktNode = ckt.node(nodeIdx)
            if cktNode.isLeaf():
                continue
            subCktIdx = self.dDB.subCkt(cktIdx).node(nodeIdx).graphIdx
            subCktName = self.dDB.subCkt(subCktIdx).name
            if magicalFlow.isImplTypeDevice(self.dDB.subCkt(subCktIdx).implType):
                continue
            cmd = "/home/unga/jayliu/projects/develop/magical/magical/install/python/bin/renameGds " \
            + self.resultName + subCktName + ".route.gds " \
            + self.dDB.subCkt(cktIdx).node(nodeIdx).name \
            + " ./gds/" + self.resultName + self.dDB.subCkt(cktIdx).node(nodeIdx).name + ".gds"
            subprocess.call(cmd, shell=True)

    def implCktLayout(self, cktIdx):
        """
        @brief implement the circuit layout
        """
        dDB = self.mDB.designDB.db #c++ database
        ckt = dDB.subCkt(cktIdx) #magicalFlow.CktGraph
        if magicalFlow.isImplTypeDevice(ckt.implType):
            Device_generator.Device_generator(self.mDB).generateDevice(cktIdx, self.resultName+'/gds/')
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
        self.stupidSetup(cktIdx)
        PnR.PnR(self.mDB).implLayout(cktIdx, self.resultName)
