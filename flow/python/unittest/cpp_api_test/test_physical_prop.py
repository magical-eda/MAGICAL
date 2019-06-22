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

        self.assertEqual(mos.mult, -1)
        self.assertFalse(mos.multValid())
        mos.mult = 3
        self.assertEqual(mos.mult, 3)
        self.assertTrue(mos.multValid())

        self.assertEqual(mos.numFingers, -1)
        self.assertFalse(mos.numFingersValid())
        mos.numFingers = 4
        self.assertEqual(mos.numFingers, 4)
        self.assertTrue(mos.numFingersValid())

        self.assertEqual(mos.sd, -1)
        self.assertFalse(mos.sdValid())
        mos.sd = 5
        self.assertEqual(mos.sd, 5)
        self.assertTrue(mos.sdValid())

        self.assertEqual(mos.ad, -1)
        self.assertFalse(mos.adValid())
        mos.ad = 6
        self.assertEqual(mos.ad, 6)
        self.assertTrue(mos.adValid())

        self.assertEqual(mos.ass, -1)
        self.assertFalse(mos.asValid())
        mos.ass = 7
        self.assertEqual(mos.ass, 7)
        self.assertTrue(mos.asValid())

        self.assertEqual(mos.pd, -1)
        self.assertFalse(mos.pdValid())
        mos.pd = 8
        self.assertEqual(mos.pd, 8)
        self.assertTrue(mos.pdValid())

        self.assertEqual(mos.ps, -1)
        self.assertFalse(mos.psValid())
        mos.ps = 9
        self.assertEqual(mos.ps, 9)
        self.assertTrue(mos.psValid())

        self.assertEqual(mos.nrd, -1)
        self.assertFalse(mos.nrdValid())
        mos.nrd = 10
        self.assertEqual(mos.nrd, 10)
        self.assertTrue(mos.nrdValid())

        self.assertEqual(mos.nrs, -1)
        self.assertFalse(mos.nrsValid())
        mos.nrs = 11
        self.assertEqual(mos.nrs, 11)
        self.assertTrue(mos.nrsValid())

        self.assertEqual(mos.sa, -1)
        self.assertFalse(mos.saValid())
        mos.sa = 12
        self.assertEqual(mos.sa, 12)
        self.assertTrue(mos.saValid())

        self.assertEqual(mos.sb, -1)
        self.assertFalse(mos.sbValid())
        mos.sb = 13
        self.assertEqual(mos.sb, 13)
        self.assertTrue(mos.sbValid())


class TestNch25ud18macProp(unittest.TestCase):
    def runTest(self):
        mos = magicalFlow.Nch25ud18macProp()
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

        self.assertEqual(mos.mult, -1)
        self.assertFalse(mos.multValid())
        mos.mult = 3
        self.assertEqual(mos.mult, 3)
        self.assertTrue(mos.multValid())

        self.assertEqual(mos.numFingers, -1)
        self.assertFalse(mos.numFingersValid())
        mos.numFingers = 4
        self.assertEqual(mos.numFingers, 4)
        self.assertTrue(mos.numFingersValid())

        self.assertEqual(mos.sd, -1)
        self.assertFalse(mos.sdValid())
        mos.sd = 5
        self.assertEqual(mos.sd, 5)
        self.assertTrue(mos.sdValid())

        self.assertEqual(mos.ad, -1)
        self.assertFalse(mos.adValid())
        mos.ad = 6
        self.assertEqual(mos.ad, 6)
        self.assertTrue(mos.adValid())

        self.assertEqual(mos.ass, -1)
        self.assertFalse(mos.asValid())
        mos.ass = 7
        self.assertEqual(mos.ass, 7)
        self.assertTrue(mos.asValid())

        self.assertEqual(mos.pd, -1)
        self.assertFalse(mos.pdValid())
        mos.pd = 8
        self.assertEqual(mos.pd, 8)
        self.assertTrue(mos.pdValid())

        self.assertEqual(mos.ps, -1)
        self.assertFalse(mos.psValid())
        mos.ps = 9
        self.assertEqual(mos.ps, 9)
        self.assertTrue(mos.psValid())

        self.assertEqual(mos.nrd, -1)
        self.assertFalse(mos.nrdValid())
        mos.nrd = 10
        self.assertEqual(mos.nrd, 10)
        self.assertTrue(mos.nrdValid())

        self.assertEqual(mos.nrs, -1)
        self.assertFalse(mos.nrsValid())
        mos.nrs = 11
        self.assertEqual(mos.nrs, 11)
        self.assertTrue(mos.nrsValid())

        self.assertEqual(mos.sa, -1)
        self.assertFalse(mos.saValid())
        mos.sa = 12
        self.assertEqual(mos.sa, 12)
        self.assertTrue(mos.saValid())

        self.assertEqual(mos.sb, -1)
        self.assertFalse(mos.sbValid())
        mos.sb = 13
        self.assertEqual(mos.sb, 13)
        self.assertTrue(mos.sbValid())

        self.assertEqual(mos.sa1, -1)
        self.assertFalse(mos.sa1Valid())
        mos.sa1 = 14
        self.assertEqual(mos.sa1, 14)
        self.assertTrue(mos.sa1Valid())

        self.assertEqual(mos.sa2, -1)
        self.assertFalse(mos.sa2Valid())
        mos.sa2 = 15
        self.assertEqual(mos.sa2, 15)
        self.assertTrue(mos.sa2Valid())

        self.assertEqual(mos.sa3, -1)
        self.assertFalse(mos.sa3Valid())
        mos.sa3 = 16
        self.assertEqual(mos.sa3, 16)
        self.assertTrue(mos.sa3Valid())

        self.assertEqual(mos.sb1, -1)
        self.assertFalse(mos.sb1Valid())
        mos.sb1 = 17
        self.assertEqual(mos.sb1, 17)
        self.assertTrue(mos.sb1Valid())

        self.assertEqual(mos.sb2, -1)
        self.assertFalse(mos.sb2Valid())
        mos.sb2 = 18
        self.assertEqual(mos.sb2, 18)
        self.assertTrue(mos.sb2Valid())

        self.assertEqual(mos.sb3, -1)
        self.assertFalse(mos.sb3Valid())
        mos.sb3 = 19
        self.assertEqual(mos.sb3, 19)
        self.assertTrue(mos.sb3Valid())


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

        self.assertEqual(mos.mult, -1)
        self.assertFalse(mos.multValid())
        mos.mult = 3
        self.assertEqual(mos.mult, 3)
        self.assertTrue(mos.multValid())

        self.assertEqual(mos.numFingers, -1)
        self.assertFalse(mos.numFingersValid())
        mos.numFingers = 4
        self.assertEqual(mos.numFingers, 4)
        self.assertTrue(mos.numFingersValid())

        self.assertEqual(mos.sd, -1)
        self.assertFalse(mos.sdValid())
        mos.sd = 5
        self.assertEqual(mos.sd, 5)
        self.assertTrue(mos.sdValid())

        self.assertEqual(mos.ad, -1)
        self.assertFalse(mos.adValid())
        mos.ad = 6
        self.assertEqual(mos.ad, 6)
        self.assertTrue(mos.adValid())

        self.assertEqual(mos.ass, -1)
        self.assertFalse(mos.asValid())
        mos.ass = 7
        self.assertEqual(mos.ass, 7)
        self.assertTrue(mos.asValid())

        self.assertEqual(mos.pd, -1)
        self.assertFalse(mos.pdValid())
        mos.pd = 8
        self.assertEqual(mos.pd, 8)
        self.assertTrue(mos.pdValid())

        self.assertEqual(mos.ps, -1)
        self.assertFalse(mos.psValid())
        mos.ps = 9
        self.assertEqual(mos.ps, 9)
        self.assertTrue(mos.psValid())

        self.assertEqual(mos.nrd, -1)
        self.assertFalse(mos.nrdValid())
        mos.nrd = 10
        self.assertEqual(mos.nrd, 10)
        self.assertTrue(mos.nrdValid())

        self.assertEqual(mos.nrs, -1)
        self.assertFalse(mos.nrsValid())
        mos.nrs = 11
        self.assertEqual(mos.nrs, 11)
        self.assertTrue(mos.nrsValid())

        self.assertEqual(mos.sa, -1)
        self.assertFalse(mos.saValid())
        mos.sa = 12
        self.assertEqual(mos.sa, 12)
        self.assertTrue(mos.saValid())

        self.assertEqual(mos.sb, -1)
        self.assertFalse(mos.sbValid())
        mos.sb = 13
        self.assertEqual(mos.sb, 13)
        self.assertTrue(mos.sbValid())

class TestRppolywoMProp(unittest.TestCase):
    def runTest(self):
        res = magicalFlow.RppolywoMProp()

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

        self.assertEqual(res.multi, -1)
        self.assertFalse(res.multiValid())
        res.multi = 3
        self.assertEqual(res.multi, 3)
        self.assertTrue(res.multiValid())

        self.assertEqual(res.m, -1)
        self.assertFalse(res.mValid())
        res.m = 4
        self.assertEqual(res.m, 4)
        self.assertTrue(res.mValid())

class TestCfmomProp(unittest.TestCase):
    def runTest(self):
        cap = magicalFlow.CfmomProp()

        self.assertEqual(cap.nr, -1)
        self.assertFalse(cap.nrValid())
        cap.nr = 1
        self.assertEqual(cap.nr, 1)
        self.assertTrue(cap.nrValid())

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

        self.assertEqual(cap.s, -1)
        self.assertFalse(cap.sValid())
        cap.s = 4
        self.assertEqual(cap.s, 4)
        self.assertTrue(cap.sValid())

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
        self.assertFalse(cap.multiValid())
        cap.multi = 7
        self.assertEqual(cap.multi, 7)
        self.assertTrue(cap.multiValid())

        self.assertEqual(cap.ftip, -1)
        self.assertFalse(cap.ftipValid())
        cap.ftip = 8
        self.assertEqual(cap.ftip, 8)
        self.assertTrue(cap.ftipValid())

if __name__ == '__main__':
    unittest.main()
