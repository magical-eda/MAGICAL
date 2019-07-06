##
# @brief Test API for TechDB
# @author Keren Zhu
# @date 07/06/2019
#

import os 
import unittest, magicalFlow


class TestTechDbAPI(unittest.TestCase):
    def setUp(self):
        self.simple_techfile = os.path.dirname(os.path.abspath(__file__)) + "/../test_data/layer.simple.tech"
        self.techDB = magicalFlow.TechDB()
    def runTest(self):
        self.parse_simple_techfile()
        self.verify_simple_tech()
    def parse_simple_techfile(self):
        magicalFlow.parseSimpleTechFile(self.simple_techfile, self.techDB)
    def verify_simple_tech(self):
        self.assertEqual(self.techDB.dbLayerToPdk(0), 1);
        self.assertEqual(self.techDB.pdkLayerToDb(1), 0);
        self.assertEqual(self.techDB.layerNameToIdx("PO"), 0);

        self.assertEqual(self.techDB.dbLayerToPdk(1), 10);
        self.assertEqual(self.techDB.pdkLayerToDb(10), 1);
        self.assertEqual(self.techDB.layerNameToIdx("CO"), 1);

        self.assertEqual(self.techDB.dbLayerToPdk(2), 11);
        self.assertEqual(self.techDB.pdkLayerToDb(11), 2);
        self.assertEqual(self.techDB.layerNameToIdx("C1"), 2);

        self.assertEqual(self.techDB.dbLayerToPdk(3), 12);
        self.assertEqual(self.techDB.pdkLayerToDb(12), 3);
        self.assertEqual(self.techDB.layerNameToIdx("C2"), 3);

        self.assertEqual(self.techDB.dbLayerToPdk(4), 13);
        self.assertEqual(self.techDB.pdkLayerToDb(13), 4);
        self.assertEqual(self.techDB.layerNameToIdx("C3"), 4);

        self.assertEqual(self.techDB.dbLayerToPdk(5), 14);
        self.assertEqual(self.techDB.pdkLayerToDb(14), 5);
        self.assertEqual(self.techDB.layerNameToIdx("C4"), 5);

        self.assertEqual(self.techDB.dbLayerToPdk(6), 21);
        self.assertEqual(self.techDB.pdkLayerToDb(21), 6);
        self.assertEqual(self.techDB.layerNameToIdx("M1"), 6);

        self.assertEqual(self.techDB.dbLayerToPdk(7), 22);
        self.assertEqual(self.techDB.pdkLayerToDb(22), 7);
        self.assertEqual(self.techDB.layerNameToIdx("M2"), 7);

        self.assertEqual(self.techDB.dbLayerToPdk(8), 23);
        self.assertEqual(self.techDB.pdkLayerToDb(23), 8);
        self.assertEqual(self.techDB.layerNameToIdx("M3"), 8);

        self.assertEqual(self.techDB.dbLayerToPdk(9), 24);
        self.assertEqual(self.techDB.pdkLayerToDb(24), 9);
        self.assertEqual(self.techDB.layerNameToIdx("M4"), 9);

        self.assertEqual(self.techDB.dbLayerToPdk(10), 25);
        self.assertEqual(self.techDB.pdkLayerToDb(25), 10);
        self.assertEqual(self.techDB.layerNameToIdx("M5"), 10);

if __name__ == '__main__':
    unittest.main()
