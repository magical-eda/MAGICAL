##
# @file PnR.py
# @author Keren Zhu
# @date 07/07/2019
# @brief The class for implementing one layout for a circuit
#


class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
    
    def implLayout(self, ckt_idx):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        return True
    def placement(self):
        """
        @brief placement
        """
        return True
    def routing(self):
        """
        @brief routing
        """
        return True
    def device_generation(self):
        """
        @brief device generation
        """
        return True
