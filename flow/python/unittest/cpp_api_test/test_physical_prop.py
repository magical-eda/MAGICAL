import unittest, magicalFlow

class TestMosProp(unittest.TestCase):
    def runTest(self):
        mos = magicalFlow.MosProp()
        self.assertEqual(mos.length, -1)
        self.assertFalse(mos.lengthValid())
        mos.length = 1
        self.assertEqual(mos.length, 1)
        self.assertTrue(mos.lengthValid())
        self.assertEqual(mos.width, -1)
        self.assertFalse(mos.widthValid())
        mos.width = 2
        self.assertEqual(mos.width, 2)
        self.assertTrue(mos.widthValid())

        self.assertEqual(mos.attr, "")
        mos.attr = "hello_world"
        self.assertEqual(mos.attr, "hello_world")

class TestNchProp(unittest.TestCase):
    def runTest(self):
        mos = magicalFlow.NchProp()
        self.assertEqual(mos.length, -1)
        self.assertFalse(mos.lengthValid())
        mos.length = 1
        self.assertEqual(mos.length, 1)
        self.assertTrue(mos.lengthValid())
        self.assertEqual(mos.width, -1)
        self.assertFalse(mos.widthValid())
        mos.width = 2
        self.assertEqual(mos.width, 2)
        self.assertTrue(mos.widthValid())

        self.assertEqual(mos.mult, 1)
#        self.assertFalse(mos.multValid())
        mos.mult = 3
        self.assertEqual(mos.mult, 3)
#        self.assertTrue(mos.multValid())

        self.assertEqual(mos.numFingers, 1)
#        self.assertFalse(mos.numFingersValid())
        mos.numFingers = 4
        self.assertEqual(mos.numFingers, 4)
#        self.assertTrue(mos.numFingersValid())

        self.assertEqual(mos.attr, "")
        mos.attr = "nch_lvt"
        self.assertEqual(mos.attr, "nch_lvt")

class TestPchProp(unittest.TestCase):
    def runTest(self):
        mos = magicalFlow.PchProp()
        self.assertEqual(mos.length, -1)
        self.assertFalse(mos.lengthValid())
        mos.length = 1
        self.assertEqual(mos.length, 1)
        self.assertTrue(mos.lengthValid())
        self.assertEqual(mos.width, -1)
        self.assertFalse(mos.widthValid())
        mos.width = 2
        self.assertEqual(mos.width, 2)
        self.assertTrue(mos.widthValid())

        self.assertEqual(mos.mult, 1)
#        self.assertFalse(mos.multValid())
        mos.mult = 3
        self.assertEqual(mos.mult, 3)
#        self.assertTrue(mos.multValid())

        self.assertEqual(mos.numFingers, 1)
#        self.assertFalse(mos.numFingersValid())
        mos.numFingers = 4
        self.assertEqual(mos.numFingers, 4)
#        self.assertTrue(mos.numFingersValid())

        self.assertEqual(mos.attr, "")
        mos.attr = "pch_lvt"
        self.assertEqual(mos.attr, "pch_lvt")

class TestResProp(unittest.TestCase):
    def runTest(self):
        res = magicalFlow.ResProp()

        self.assertEqual(res.lr, -1)
        self.assertFalse(res.lrValid())
        res.lr = 1
        self.assertEqual(res.lr, 1)
        self.assertTrue(res.lrValid())

        self.assertEqual(res.wr, -1)
        self.assertFalse(res.wrValid())
        res.wr = 2
        self.assertEqual(res.wr, 2)
        self.assertTrue(res.wrValid())

        self.assertFalse(res.series)
        self.assertFalse(res.parallel)
        res.series = True
        res.parallel = True
        self.assertTrue(res.series)
        self.assertTrue(res.parallel)
        
        self.assertEqual(res.segNum, 1)
        res.segNum = 3
        self.assertEqual(res.segNum, 3)

        self.assertEqual(res.segSpace, -1)
        res.segSpace = 180
        self.assertEqual(res.segSpace, 180)

        self.assertEqual(res.attr, "")
        res.attr = "rppolyw_mo"
        self.assertEqual(res.attr, "rppolyw_mo")

class TestCapProp(unittest.TestCase):
    def runTest(self):
        cap = magicalFlow.CapProp()

        self.assertEqual(cap.numFingers, 1)
        cap.numFingers = 3
        self.assertEqual(cap.numFingers, 3)

        self.assertEqual(cap.lr, -1)
        self.assertFalse(cap.lrValid())
        cap.lr = 2
        self.assertEqual(cap.lr, 2)
        self.assertTrue(cap.lrValid())

        self.assertEqual(cap.w, -1)
        self.assertFalse(cap.wValid())
        cap.w = 3
        self.assertEqual(cap.w, 3)
        self.assertTrue(cap.wValid())

        self.assertEqual(cap.spacing, -1)
        self.assertFalse(cap.spacingValid())
        cap.spacing = 4
        self.assertEqual(cap.spacing, 4)
        self.assertTrue(cap.spacingValid())

        self.assertEqual(cap.stm, -1)
        self.assertFalse(cap.stmValid())
        cap.stm = 5
        self.assertEqual(cap.stm, 5)
        self.assertTrue(cap.stmValid())

        self.assertEqual(cap.spm, -1)
        self.assertFalse(cap.spmValid())
        cap.spm = 6
        self.assertEqual(cap.spm, 6)
        self.assertTrue(cap.spmValid())

        self.assertEqual(cap.multi, -1)
        cap.multi = 7
        self.assertEqual(cap.multi, 7)

        self.assertEqual(cap.ftip, -1)
        self.assertFalse(cap.ftipValid())
        cap.ftip = 8
        self.assertEqual(cap.ftip, 8)
        self.assertTrue(cap.ftipValid())

        self.assertEqual(cap.attr, "")
        cap.attr = "cfmom_2t"
        self.assertEqual(cap.attr, "cfmom_2t")

if __name__ == '__main__':
    unittest.main()
