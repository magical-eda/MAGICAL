##
# @file WellMgr.py
# @author Keren Zhu
# @date 03/14/2021
# @brief Well generation manager
#

import magicalFlow
from device_generation.basic import basic as basic

class WellMgr(object):
    def __init__(self, ddb, tdb):
        """
        @param ddb: a magicalFlow.DesignDB object
        """
        self._ddb = ddb
        self._tdb = tdb
        self._util = magicalFlow.DataWellGAN(ddb, tdb.pdkLayerToDb(basic.layer['OD']))
    def clear(self):
        self._util.clear()
    def constructCkt(self, cktIdx):
        self._util.construct(cktIdx)
