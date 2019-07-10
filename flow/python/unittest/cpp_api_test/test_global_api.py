"""
@author Keren Zhu, Mingjie Liu
@brief test the global api
"""

import unittest, magicalFlow

class TestImplType(unittest.TestCase):
    def runTest(self):
        self.testIsImplTypeDevice()
    def testIsImplTypeDevice(self):
        self.assertFalse(magicalFlow.isImplTypeDevice(magicalFlow.ImplTypeUNSET))
        self.assertTrue(magicalFlow.isImplTypeDevice(magicalFlow.ImplTypePCELL_Pch))
        self.assertTrue(magicalFlow.isImplTypeDevice(magicalFlow.ImplTypePCELL_Nch))
        self.assertTrue(magicalFlow.isImplTypeDevice(magicalFlow.ImplTypePCELL_Res))
        self.assertTrue(magicalFlow.isImplTypeDevice(magicalFlow.ImplTypePCELL_Cap))

class TestOriType(unittest.TestCase):
    def runTest(self):
        self.assertNotEqual(magicalFlow.OriTypeN, magicalFlow.OriTypeS)

class TestConvOri(unittest.TestCase):
    def setUp(self):
        self.bbox = magicalFlow.BoxLoc()
        self.bbox.xLo = 0
        self.bbox.yLo = 0
        self.bbox.xHi = 100
        self.bbox.yHi = 200
        self.offset = magicalFlow.XYLoc()
        self.offset.x = 3
        self.offset.y = 5
        self.ll = magicalFlow.XYLoc()
        self.ll.x = 0
        self.ll.y = 0
        self.lr = magicalFlow.XYLoc()
        self.lr.x = 100
        self.lr.y = 0
        self.ul = magicalFlow.XYLoc()
        self.ul.x = 0
        self.ul.y = 200
        self.ur = magicalFlow.XYLoc()
        self.ur.x = 100
        self.ur.y = 200

    def runTest(self):
        self.testNorth()
        self.testSouth()
        self.testWest()
        self.testEast()
        self.testFN()
        self.testFS()
        self.testFW()
        self.testFE()

    def testDefault(self):
        default = magicalFlow.orientConv()
        self.assertEqual(default.x, 0)
        self.assertEqual(default.y, 0)
        default_35 = magicalFlow.orientConv(coord=self.offset)
        self.assertEqual(default_35.x, 3)
        self.assertEqual(default_35.y, 5)

    def testNorth(self):
        ori = magicalFlow.OriTypeN
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 3)
        self.assertEqual(ll_.y, 5)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 103)
        self.assertEqual(lr_.y, 5)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 3)
        self.assertEqual(ul_.y, 205)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 103)
        self.assertEqual(ur_.y, 205)

    def testSouth(self):
        ori = magicalFlow.OriTypeS
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 103)
        self.assertEqual(ll_.y, 205)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 3)
        self.assertEqual(lr_.y, 205)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 103)
        self.assertEqual(ul_.y, 5)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 3)
        self.assertEqual(ur_.y, 5)

    def testWest(self):
        ori = magicalFlow.OriTypeW
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 203)
        self.assertEqual(ll_.y, 5)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 203)
        self.assertEqual(lr_.y, 105)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 3)
        self.assertEqual(ul_.y, 5)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 3)
        self.assertEqual(ur_.y, 105)

    def testEast(self):
        ori = magicalFlow.OriTypeE
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 3)
        self.assertEqual(ll_.y, 105)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 3)
        self.assertEqual(lr_.y, 5)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 203)
        self.assertEqual(ul_.y, 105)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 203)
        self.assertEqual(ur_.y, 5)

    def testFN(self):
        ori = magicalFlow.OriTypeFN
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 103)
        self.assertEqual(ll_.y, 5)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 3)
        self.assertEqual(lr_.y, 5)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 103)
        self.assertEqual(ul_.y, 205)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 3)
        self.assertEqual(ur_.y, 205)

    def testFS(self):
        ori = magicalFlow.OriTypeFS
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 3)
        self.assertEqual(ll_.y, 205)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 103)
        self.assertEqual(lr_.y, 205)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 3)
        self.assertEqual(ul_.y, 5)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 103)
        self.assertEqual(ur_.y, 5)

    def testFW(self):
        ori = magicalFlow.OriTypeFW
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 3)
        self.assertEqual(ll_.y, 5)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 3)
        self.assertEqual(lr_.y, 105)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 203)
        self.assertEqual(ul_.y, 5)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 203)
        self.assertEqual(ur_.y, 105)

    def testFE(self):
        ori = magicalFlow.OriTypeFE
        ll_ = magicalFlow.orientConv(self.ll, ori, self.offset, self.bbox)
        self.assertEqual(ll_.x, 203)
        self.assertEqual(ll_.y, 105)
        lr_ = magicalFlow.orientConv(self.lr, ori, self.offset, self.bbox)
        self.assertEqual(lr_.x, 203)
        self.assertEqual(lr_.y, 5)
        ul_ = magicalFlow.orientConv(self.ul, ori, self.offset, self.bbox)
        self.assertEqual(ul_.x, 3)
        self.assertEqual(ul_.y, 105)
        ur_ = magicalFlow.orientConv(self.ur, ori, self.offset, self.bbox)
        self.assertEqual(ur_.x, 3)
        self.assertEqual(ur_.y, 5)



if __name__ == '__main__':
    unittest.main()
