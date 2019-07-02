##
# @brief test the API for DesignDB
# @author Keren Zhu
# @date 06/27/2019
#

import unittest, magicalFlow

class TestDesignDB(unittest.TestCase):
    def runTest(self):
        self.find_root_test()
    def find_root_test(self):
        db = magicalFlow.DesignDB()
        for i in range(0, 7):
            db.allocateCkt();
        idx = db.subCkt(5).allocateNode();
        db.subCkt(5).node(idx).graphIdx = 2;
        idx = db.subCkt(5).allocateNode();
        db.subCkt(5).node(idx).graphIdx = 0;
        idx = db.subCkt(4).allocateNode();
        db.subCkt(4).node(idx).graphIdx =  0;
        idx = db.subCkt(4).allocateNode();
        db.subCkt(4).node(idx).graphIdx = 1;
        idx = db.subCkt(2).allocateNode();
        db.subCkt(2).node(idx).graphIdx = 3;
        idx = db.subCkt(3).allocateNode();
        db.subCkt(3).node(idx).graphIdx = 1;
        idx = db.subCkt(6).allocateNode();
        db.subCkt(6).node(idx).graphIdx = 5;
        idx = db.subCkt(6).allocateNode();
        db.subCkt(6).node(idx).graphIdx = 4;
        db.findRootCkt()
        self.assertEqual(db.rootCktIdx(), 6)

if __name__ == '__main__':
    unittest.main()
