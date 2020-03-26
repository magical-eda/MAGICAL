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
        self.parse_input_netlist(self.params)
        self.parse_simple_techfile('/home/unga/jayliu/projects/inputs/techfile.simple')
        self.designDB.db.findRootCkt() # After the parsing, find the root circuit of the hierarchy
        self.postProcessing()
        return True

    def postProcessing(self):
        self.markPowerNets()

    def parse_simple_techfile(self, params):
        magicalFlow.parseSimpleTechFile( params, self.techDB)

    def parse_input_netlist(self, params):
        if (params.hspice_netlist is not None):
            self.read_hspice_netlist(params.resultDir+params.hspice_netlist)
            return
        if (params.spectre_netlist is not None):
            self.read_spectre_netlist(params.resultDir+params.spectre_netlist)
            return
        raise ParamException("No input netlist file!")

    def read_spectre_netlist(self, sp_netlist):
        self.designDB.read_spectre_netlist(sp_netlist)

    def read_hspice_netlist(self, sp_netlist):
        self.designDB.read_hspice_netlist(sp_netlist)

    """
    Post-processing
    """
    def markPowerNets(self):
        for cktIdx in range(self.designDB.db.numCkts()):
            ckt = self.designDB.db.subCkt(cktIdx)
            # using flags from body connections
            for psubIdx in range(ckt.numPsubs()):
                psubNet = ckt.psub(psubIdx)
                psubNet.markVssFlag()
            for nwellIdx in range(ckt.numNwells()):
                nwellNet = ckt.nwell(nwellIdx)
                nwellNet.markVddFlag()
            # Using external naming-based labeling
            vddNetNames = self.params.vddNetNames
            vssNetNames = self.params.vssNetNames
            for netIdx in range(ckt.numNets()):
                net = ckt.net(netIdx)
                if net.name in vddNetNames:
                    net.markVddFlag()
                if net.name in vssNetNames:
                    net.markVssFlag()

    """
    Some wrapper
    """
    def topCktIdx(self):
        """
        @brief Get the index of circuit graph of the hierarchy
        """
        return self.designDB.db.rootCktIdx()
    """
    utility
    """
    def implTypeStr(self, implType):
        if implType == magicalFlow.ImplTypeUNSET:
            return "UNSET"
        if implType == magicalFlow.ImplTypePCELL_Cap:
            return "PCELLL_CAP"
        if implType ==  magicalFlow.ImplTypePCELL_Res:
            return "PCELL_RES"
        if implType == magicalFlow.ImplTypePCELL_Nch:
            return "PCELL_NCH"
        if implType == magicalFlow.ImplTypePCELL_Pch:
            return "PCELL_PCH"
        return "UNKNOWN"
