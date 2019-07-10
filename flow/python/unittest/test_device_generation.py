##
# @file test_device_generation.py
# @author Mingjie Liu
# @date 06/28/2019
# @brief Unittest for netlist parsers
# Unittest not supported for gdspy
#

import os 
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import DesignDB as pyDB
import magicalFlow
import Device_generator as devGen

class TestNetlistParser():
    def setUp(self):
        self.spectre_comparator = os.path.dirname(os.path.abspath(__file__)) + "/test_data/spectre_netlists/Comparator.sp"
        self.out_dir = os.path.dirname(os.path.abspath(__file__)) + "/test_data/Comparator/"
    def runTest(self):
        self.spectre_devgen_comparator_test()
    def spectre_devgen_comparator_test(self):
        db = magicalFlow.DesignDB()
        nlp = pyDB.Netlist_parser(db)
        nlp.parse_spectre(self.spectre_comparator)
        db.findRootCkt()
        topckt = db.subCkt(db.rootCktIdx())
        devgen = devGen.Device_generator(db)
        for node_idx in range(topckt.numNodes()):
            node = topckt.node(node_idx)
            devgen.setGDS(self.out_dir+node.name+'.gds')
            devgen.setPinBB(self.out_dir+node.name+'.pin')
            devgen.generateDevice(node.graphIdx)

if __name__ == '__main__':
    x = TestNetlistParser()
    x.setUp()
    x.runTest()
