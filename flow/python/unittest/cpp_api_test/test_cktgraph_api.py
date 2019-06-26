"""
@brief Test the CktGraph API
@author Keren Zhu
@date 06/25/2019
"""

import unittest, magicalFlow
import test_graph_components

class TestCktGraph(unittest.TestCase):
    def setUp(self):
        self.ckt = magicalFlow.CktGraph()
    def runTest(self):
        self.node_test()
        self.pin_test()
        self.net_test()
    def node_test(self):
        self.assertEqual(self.ckt.numNodes(), 0)
        idx = self.ckt.allocateNode()
        self.assertEqual(idx, 0)
        self.assertEqual(self.ckt.numNodes(), 1)

        self.assertTrue(self.ckt.node(0).isLeaf())
        self.ckt.node(0).graphIdx = 1
        self.assertEqual(self.ckt.node(0).graphIdx, 1)
        self.assertFalse(self.ckt.node(0).isLeaf())

        self.assertEqual(self.ckt.node(0).numPins(), 0)
        self.ckt.node(0).appendPinIdx(2)
        self.assertEqual(self.ckt.node(0).numPins(), 1)
        self.assertEqual(self.ckt.node(0).pinIdx(0), 2)

        self.assertFalse(self.ckt.node(0).isImpl)
        self.ckt.node(0).isImpl = True
        self.assertTrue(self.ckt.node(0).isImpl)

        self.assertEqual(self.ckt.node(0).offset().x, 0)
        self.assertEqual(self.ckt.node(0).offset().y, 0)
        self.ckt.node(0).offset().x = -1
        self.assertEqual(self.ckt.node(0).offset().x, -1)
        self.ckt.node(0).offset().y = -2
        self.assertEqual(self.ckt.node(0).offset().y, -2)

        self.assertEqual(self.ckt.node(0).orient(), magicalFlow.OriTypeN)
        self.ckt.node(0).setOrient(magicalFlow.OriTypeE)
        self.assertEqual(self.ckt.node(0).orient(), magicalFlow.OriTypeE)
    def pin_test(self):
        self.assertEqual(self.ckt.numPins(), 0)
        idx = self.ckt.allocatePin()
        self.assertEqual(idx, 0)
        self.assertEqual(self.ckt.numPins(), 1)

        self.assertNotEqual(self.ckt.pin(0).nodeIdx, 1)
        self.ckt.pin(0).nodeIdx = 1
        self.assertEqual(self.ckt.pin(0).nodeIdx, 1)

        self.assertNotEqual(self.ckt.pin(0).intPinIdx, 2)
        self.ckt.pin(0).intPinIdx = 2
        self.assertEqual(self.ckt.pin(0).intPinIdx, 2)

        self.assertEqual(self.ckt.pin(0).numNets(), 0)
        self.ckt.pin(0).appendNetIdx(3)
        self.assertEqual(self.ckt.pin(0).numNets(), 1)
        self.assertEqual(self.ckt.pin(0).netIdx(0), 3)

    def net_test(self):
        self.assertEqual(self.ckt.numNets(), 0)
        idx = self.ckt.allocateNet()
        self.assertEqual(idx, 0)
        self.assertEqual(self.ckt.numNets(), 1)

        self.assertEqual(self.ckt.net(0).numPins(), 0)
        self.ckt.net(0).appendPinIdx(2)
        self.assertEqual(self.ckt.net(0).numPins(), 1)
        self.assertEqual(self.ckt.net(0).pinIdx(0), 2)



if __name__ == '__main__':
    unittest.main()
