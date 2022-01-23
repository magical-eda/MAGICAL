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
import time


class Flow(object):
    def __init__(self, db):
        self.mDB = db # MagicalDB
        self.dDB = db.designDB.db
        self.constraint = Constraint.Constraint(self.mDB)
        self.params = self.mDB.params
        self.pnrs = []
        self.runtime = 0

    def run(self):
        """
        @brief the main function to run the flow
        @return if successful
        """
        self.resultName = self.mDB.params.resultDir
        topCktIdx = self.mDB.topCktIdx() # The index of the topckt
        start = time.time()
        self.implCktLayout(topCktIdx)
        end = time.time()
        print("runtime ", end - start)
        for pnr in self.pnrs:
            pnr.routeOnly()
        self.translateGdsLayer(self.pnrs[-1].gdsFile, self.pnrs[-1].gdsFile+"_cor") # Some dirty issue about layer ID and datatype
        return True

    def generateConstraints(self):
        for cktIdx in range(self.dDB.numCkts()):
            ckt = self.dDB.subCkt(cktIdx) #magicalFlow.CktGraph
            if magicalFlow.isImplTypeDevice(ckt.implType):
                continue
            if self.isCktStdCells(cktIdx):
                continue
            self.symDict = self.constraint.genConstraint(cktIdx, self.resultName)


    def isCktStdCells(self, cktIdx):
        if self.dDB.subCkt(cktIdx).name in self.params.stdCells:
            return True
        else:
            return False


    def setup(self, cktIdx):
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
            devGen = Device_generator.Device_generator(self.mDB)
            if magicalFlow.isImplTypeDevice(self.dDB.subCkt(subCktIdx).implType):
                if flipCell:
                    devGen.generateDevice(subCktIdx, self.resultName+'/gds/', True) #FIXME: directly add to the database
                else:
                    devGen.generateDevice(subCktIdx, self.resultName+'/gds/', False)
                devGen.readGDS(subCktIdx, self.resultName+'/gds/')
            else:
                if flipCell:
                    cktNode.flipVertFlag = True

    def implCktLayout(self, cktIdx):
        """
        @brief implement the circuit layout
        """
        dDB = self.mDB.designDB.db #c++ database
        ckt = dDB.subCkt(cktIdx) #magicalFlow.CktGraph
        # If the ckt is a device, generation will be added in setup()
        if magicalFlow.isImplTypeDevice(ckt.implType):
            Device_generator.Device_generator(self.mDB).generateDevice(cktIdx, self.resultName+'/gds/') #FIXME: directly add to the database
            return
        # If the ckt is a standard cell
        # This version only support DFCNQD2BWP and NR2D8BWP, hard-encoded
        # TODO: This should be parsed from the json file
        if self.isCktStdCells(cktIdx):
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
            ckt = dDB.subCkt(cktIdx) # just to make sure the reference is not messed up
        # After all the children being implemented. P&R at this circuit
        ckt = dDB.subCkt(cktIdx) #magicalFlow.CktGraph
        self.symDict = self.constraint.genConstraint(cktIdx, self.resultName)
        self.setup(cktIdx)
        pnr = PnR.PnR(self.mDB)
        pnr.placeOnly(cktIdx, self.resultName)
        self.runtime += pnr.runtime
        self.pnrs.append(pnr)
        #PnR.PnR(self.mDB).implLayout(cktIdx, self.resultName)

    def translateGdsLayer(self, inputGds, outputGds):
        """ This is for translating the <LayerID, datatype> in the resulting GDS file.
        In TSMC, different layers are assigned to different layer ID, e.g., 31 for M1 and 32 for M2.
        So we assume the layer IDs are distinct for different layers.
        But in Skywater, they decided to choose to use the same IDs for a lot of different layers and use different datatypes to distinguish them.
        To natively support this convention, we have to change the codes for the entire flow (magicalFlow, placer and router), so that the layers are indexed by <layer ID, datatype>.

        However, the bosses decide to only give me a few of days. 
        Therefore, instead of natively support the feature, we just use the distinct layer ID throughout the flow and translate the GDS file at the end.
        """
        if self.params.gdsLayerMappingFile == "":
            return
        gdsMappingFile = self.params.gdsLayerMappingFile
        magicalFlow.translateGdsLayer(gdsMappingFile, inputGds, outputGds)
        print("Right", gdsMappingFile)
