##
# @file WellMgr.py
# @author Keren Zhu
# @date 03/14/2021
# @brief Well generation manager
#

import magicalFlow
from device_generation.basic as basic

class OdLayer(object):
    """
    @brief record the shapes in OD layer
    """
    def __init__(self):
        self.nwShapes = [] # N well shape
        self.odOutsideShapes = [] # OD shapes outside n well
        self.odInsideShapes = [] # OD shapes indie n well
    def clear(self):
        self.nwShapes.clear()
        self.odOutsideShapes.clear()
        self.odInsideShapes.clear()

class WellMgr(object):
    def __init__(self, ddb):
        """
        @param ddb: a magicalFlow.DesignDB object
        """
        self._ddb = ddb
    def initCkt(self, cktIdx):
