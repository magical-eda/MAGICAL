##
# @file test_netlist_parser.py
# @author Keren Zhu
# @date 06/28/2019
# @brief Unittest for netlist parsers
#

import os 
import sys
import glob
import unittest
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
import DesignDB as pyDB
import magicalFlow

class TestNetlistParser(unittest.TestCase):
    def setUp(self):
        self.spectre_comparator = os.path.dirname(os.path.abspath(__file__)) + "/../test_data/spectre_netlists/Comparator.sp"
    def runTest(self):
        self.spectre_parser_comparator_test()
    def spectre_parser_comparator_test(self):
        db = magicalFlow.DesignDB()
        nlp = pyDB.Netlist_parser(db)
        nlp.parse_spectre(self.spectre_comparator)
        self.assertEqual(db.numCkts(), 18)
        for ckt_idx in range(db.numCkts()):
            ckt = db.subCkt(ckt_idx)
            print("Ckt ", ckt.name)
            print("# of nodes ", ckt.numNodes())
            for node_idx in range(ckt.numNodes()):
                node = ckt.node(node_idx)
                print("Node ", node.name, " subgraph index ", node.graphIdx)
        db.findRootCkt()
        topckt = db.subCkt(db.rootCktIdx())
        print(topckt.name)
        self.assertEqual(db.rootCktIdx(), 0) # topcircuit is node 0. others are CMOS
        for node_idx in range(topckt.numNodes()):
            node = topckt.node(node_idx)
        return True
    def check_cmos_net(ckt_idx, mos_node_idx, db, first, second, third, fourth):
        """
        @brief check the cmos is connected to the correct nets
        """
        ckt = db.subCkt(ckt_idx)

        pin1_idx = ckt.node(mos_node_idx).pinIdx(0)
        net1_idx = ckt.pin(pin1_idx).netIdx
        net1 = ckt.net(net1_idx)
        self.assertEqual(net1.name, first)

        pin2_idx = ckt.node(mos_node_idx).pinIdx(1)
        net2_idx = ckt.pin(pin2_idx).netIdx
        net2 = ckt.net(net2_idx)
        self.assertEqual(net2.name, second)

        pin3_idx = ckt.node(mos_node_idx).pinIdx(2)
        net3_idx = ckt.pin(pin3_idx).netIdx
        net3 = ckt.net(net3_idx)
        self.assertEqual(net3.name, third)

        pin4_idx = ckt.node(mos_node_idx).pinIdx(3)
        net4_idx = ckt.pin(pin4_idx).netIdx
        net4 = ckt.net(net4_idx)
        self.assertEqual(net4.name, third)

if __name__ == '__main__':
    unittest.main()
