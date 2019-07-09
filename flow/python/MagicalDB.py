##
# @file MagicalDB.py
# @author Keren Zhu
# @date 06/27/2019
# @brief The database for the magical flow. Ideally it should include everything needed
#

import DesignDB
import magicalFlow

class MagicalDB(object):
    def __init__(self, params):
        self.designDB = DesignDB.DesignDB()
        self.params = params
        self.techDB = magicalFlow.TechDB()

    def parse(self):
        self.parse_simple_techfile(self.params)
        self.parse_input_netlist(self.params)
        self.designDB.db.findRootCkt() # After the parsing, find the root circuit of the hierarchy
        return True

    def parse_simple_techfile(self, params):
        magicalFlow.parseSimpleTechFile( params.simple_tech_file, self.techDB)

    def parse_input_netlist(self, params):
        if (params.hspice_netlist is not None):
            self.read_hspice_netlist(params.hspice_netlist)
            return
        if (params.spectre_netlist is not None):
            self.read_spectre_netlist(params.spectre_netlist)
            return
        raise ParamException("No input netlist file!")

    def read_spectre_netlist(self, sp_netlist):
        self.designDB.read_spectre_netlist(sp_netlist)

    def read_hspice_netlist(self, sp_netlist):
        self.designDB.read_hspice_netlist(sp_netlist)

    """
    Some wrapper
    """
    def topCktIdx(self):
        """
        @brief Get the index of circuit graph of the hierarchy
        """
        return self.designDB.db.rootCktIdx()
