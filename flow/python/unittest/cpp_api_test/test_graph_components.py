import unittest, magicalFlow

class TestCktNode(unittest.TestCase):
    def runTest(self):
        cktNode = magicalFlow.CktNode()

        self.assertEqual(cktNode.graphIdx, 4294967295)
        cktNode.graphIdx = 1
        self.assertEqual(cktNode.graphIdx, 1)

        self.assertEqual(cktNode.numPins(), 0)
        cktNode.appendPinIdx(2)
        self.assertEqual(cktNode.numPins(), 1)
        self.assertEqual(cktNode.pinIdx(0), 2)

        self.assertFalse(cktNode.isImpl)
        cktNode.isImpl = True
        self.assertTrue(cktNode.isImpl)

if __name__ == '__main__':
    unittest.main()
