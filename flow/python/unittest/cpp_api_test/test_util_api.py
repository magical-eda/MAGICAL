"""
@author Keren Zhu
@brief test the util api
"""

import unittest, magicalFlow

class TestXYType(unittest.TestCase):
    def runTest(self):
        self.testXYLoc()
        self.testXYIdx()
    def testXYLoc(self):
        xy = magicalFlow.XYLoc()
        xy.x = 100
        xy.y = 200
        self.assertEqual(xy.x, 100)
        self.assertEqual(xy.y, 200)
        xy.x = -100
        xy.y = -200
        self.assertEqual(xy.x, -100)
        self.assertEqual(xy.y, -200)
    def testXYIdx(self):
        xy = magicalFlow.XYIdx()
        xy.x = 100
        xy.y = 200
        self.assertEqual(xy.x, 100)
        self.assertEqual(xy.y, 200)

class TestBoxType(unittest.TestCase):
    def runTest(self):
        self.testBoxLoc()
    def testBoxLoc(self):
        box = magicalFlow.BoxLoc()
        box.xLo = 1
        box.yLo = 2
        box.xHi = 11
        box.yHi = 22
        self.assertEqual(box.xLo, 1)
        self.assertEqual(box.yLo, 2)
        self.assertEqual(box.xHi, 11)
        self.assertEqual(box.yHi, 22)
        self.assertEqual(box.xLen(), 10)
        self.assertEqual(box.yLen(), 20)

if __name__ == '__main__':
    unittest.main()
