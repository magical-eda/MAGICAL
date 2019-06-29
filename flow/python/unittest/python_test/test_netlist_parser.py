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
        self.spectre_netlists = glob.glob("../test_data/spectre_netlists/*.sp")
    def runTest(self):
        self.spectre_parser_test()
    def spectre_parser_test(self):
        db = magicalFlow.DesignDB()
        nlp = pyDB.Netlist_parser(db)
        for spec in self.spectre_netlists:
            nlp.parse_spectre(spec)
        return True

if __name__ == '__main__':
    unittest.main()
