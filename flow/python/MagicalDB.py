##
# @file MagicalDB.py
# @author Keren Zhu
# @date 06/27/2019
# @brief The database for the magical flow. Ideally it should include everything needed
#

import DesignDB

class MagicalDB(object):
    def __init__(self):
        self.designDB = DesignDB()

    def read_spectre_netlist(self, sp_netlist):
        self.designDB.read_sp_netlist()
