"""
@brief Test the graph component API
@author Keren Zhu
@date 06/24/2019
"""

import unittest, magicalFlow

class TestCktNode(unittest.TestCase):
    def runTest(self):
        node = magicalFlow.CktNode()
        self.node_test(node)
    def node_test(self, cktNode):
        self.assertTrue(cktNode.isLeaf())
        cktNode.graphIdx = 1
        self.assertEqual(cktNode.graphIdx, 1)
        self.assertFalse(cktNode.isLeaf())

        self.assertEqual(cktNode.numPins(), 0)
        cktNode.appendPinIdx(2)
        self.assertEqual(cktNode.numPins(), 1)
        self.assertEqual(cktNode.pinIdx(0), 2)

        self.assertFalse(cktNode.isImpl)
        cktNode.isImpl = True
        self.assertTrue(cktNode.isImpl)

        self.assertEqual(cktNode.offset().x, 0)
        self.assertEqual(cktNode.offset().y, 0)
        cktNode.offset().x = -1
        self.assertEqual(cktNode.offset().x, -1)
        cktNode.offset().y = -2
        self.assertEqual(cktNode.offset().y, -2)

        self.assertEqual(cktNode.orient(), magicalFlow.OriTypeN)
        cktNode.setOrient(magicalFlow.OriTypeE)
        self.assertEqual(cktNode.orient(), magicalFlow.OriTypeE)

        self.assertEqual(cktNode.implType, magicalFlow.ImplTypeUNSET)
        cktNode.implType = magicalFlow.ImplTypePCELL_Pch
        self.assertEqual(cktNode.implType, magicalFlow.ImplTypePCELL_Pch)


class TestNet(unittest.TestCase):
    def runTest(self):
        net = magicalFlow.Net()

        self.assertEqual(net.numPins(), 0)
        net.appendPinIdx(2)
        self.assertEqual(net.numPins(), 1)
        self.assertEqual(net.pinIdx(0), 2)

class TestPin(unittest.TestCase):
    def runTest(self):
        pin = magicalFlow.Pin()

        self.assertNotEqual(pin.nodeIdx, 1)
        pin.nodeIdx = 1
        self.assertEqual(pin.nodeIdx, 1)

        self.assertNotEqual(pin.intPinIdx, 2)
        pin.intPinIdx = 2
        self.assertEqual(pin.intPinIdx, 2)

        self.assertEqual(pin.numNets(), 0)
        pin.appendNetIdx(3)
        self.assertEqual(pin.numNets(), 1)
        self.assertEqual(pin.netIdx(0), 3)

if __name__ == '__main__':
    unittest.main()
