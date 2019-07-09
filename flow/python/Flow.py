##
# @file Flow.py
# @author Keren Zhu
# @date 07/07/2019
# @brief The flow
#

import magicalFlow
import Device_generator
import PnR

class Flow(object):
    def __init__(self, db):
        self.mDB = db # MagicalDB

    def run(self):
        """
        @brief the main function to run the flow
        @return if successful
        """
        topCktIdx = self.mDB.topCktIdx() # The index of the topckt
        self.implCktLayout(topCktIdx)
        return True
    def implCktLayout(self, cktIdx):
        """
        @brief implement the circuit layout
        """
        dDB = self.mDB.designDB.db #c++ database
        ckt = dDB.subckt(cktIdx) #magicalFlow.CktGraph
        if magicalFlow.isImplTypeDevice(ckt.implType):
            Device_generator.Device_generator(self.mDB).generateDevice(cktIdx)
            return
        # If the ckt is actually a circuit instead of a device
        for nodeIdx in range(ckt.numNodes()):
            cktNode = dDB.node(nodeIdx) # magicalFlow.CktNode
            if (cktNode.isImpl()): # Do not duplicately implement the layout
                continue
            if cktNode.isLeaf(): # Do not go deeper for leaf node
                continue
            self.implCktLayout(self, cktNode) # Recursively implement all the children
        # After all the children being implemented. P&R at this circuit
        PnR.PnR(self.mDB).implLayout(cktIdx)
