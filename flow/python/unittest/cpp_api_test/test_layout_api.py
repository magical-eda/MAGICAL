##
# @file test_layout_api.py
# @brief Test the Layout API
# @author Keren Zhu
# @date 07/03/2019
#

import unittest, magicalFlow

class TestTextLayout(unittest.TestCase):
    def runTest(self):
        text = magicalFlow.TextLayout()
        self.assertEqual(text.text, "")
        text.text = "random"
        self.assertEqual(text.text, "random")
        self.assertEqual(text.coord().x, 0)
        text.coord().x = 1
        self.assertEqual(text.coord().x, 1)
        coord = text.coord()
        coord.x = 2
        self.assertEqual(text.coord().x, 2)
        self.assertEqual(text.coord().y, 0)
        text.coord().y = 1
        self.assertEqual(text.coord().y, 1)
        coord = text.coord()
        coord.y = 2
        self.assertEqual(text.coord().y, 2)

        """
        self.assertNotEqual(text.layer, 11)
        text.layer = 11
        self.assertEqual(text.layer, 11)
        """

class TestRectLayout(unittest.TestCase):
    def runTest(self):
        rect = magicalFlow.RectLayout()
        """
        self.assertNotEqual(rect.layer, 11)
        rect.layer = 11
        self.assertEqual(rect.layer, 11)
        """
        rect.rect().xLo = 5
        self.assertEqual(rect.rect().xLo, 5)
        rect.rect().yLo = 6
        self.assertEqual(rect.rect().yLo, 6)
        rect.rect().xHi = 7
        self.assertEqual(rect.rect().xHi, 7)
        rect.rect().yHi = 8
        self.assertEqual(rect.rect().yHi, 8)

        box = rect.rect()
        box.xLo = 15
        self.assertEqual(rect.rect().xLo, 15)
        box.yLo = 16
        self.assertEqual(rect.rect().yLo, 16)
        box.xHi = 17
        self.assertEqual(rect.rect().xHi, 17)
        box.yHi = 18
        self.assertEqual(rect.rect().yHi, 18)


class TestLayout(unittest.TestCase):
    def setUp(self):
        self.layout = magicalFlow.Layout()
        self.layout.init(3)
    def runTest(self):
        self.insert_rect_test()
        self.insert_text_test()
    def insert_rect_test(self):
        initBox = magicalFlow.BoxLoc()
        initBox.xLo = 0
        initBox.xHi = 1
        initBox.yLo = 0
        initBox.yHi = 1
        firstIdx = self.layout.insertRect(0, initBox)
        self.assertEqual(firstIdx, 0)
        box = self.layout.rect(0, firstIdx).rect()
        self.assertEqual(box.xLo, 0)
        self.assertEqual(box.yLo, 0)
        self.assertEqual(box.xHi, 1)
        self.assertEqual(box.yHi, 1)
        box.xLo = 50
        self.assertEqual(self.layout.rect(0, firstIdx).rect().xLo, 50)

        rectBox = magicalFlow.RectLayout()
        rectBox.rect().xLo = 11
        secondIdx = self.layout.insertRect(0, rectBox)
        self.assertEqual(secondIdx, 1)
        self.assertEqual(self.layout.rect(0, secondIdx).rect().xLo, 11)

        lo = magicalFlow.XYLoc()
        lo.x = 12
        lo.y = 13
        hi = magicalFlow.XYLoc()
        hi.x = 22
        hi.y = 23
        thirdIdx = self.layout.insertRect(2, lo, hi)
        self.assertEqual(thirdIdx, 0)
        self.assertEqual(self.layout.rect(2, thirdIdx).rect().xLo, 12)
        self.assertEqual(self.layout.rect(2, thirdIdx).rect().yLo, 13)
        self.assertEqual(self.layout.rect(2, thirdIdx).rect().xHi, 22)
        self.assertEqual(self.layout.rect(2, thirdIdx).rect().yHi, 23)

        fourthIdx = self.layout.insertRect(2, 12, 13, 22, 23)
        self.assertEqual(fourthIdx, 1)
        self.assertEqual(self.layout.rect(2, fourthIdx).rect().xLo, 12)
        self.assertEqual(self.layout.rect(2, fourthIdx).rect().yLo, 13)
        self.assertEqual(self.layout.rect(2, fourthIdx).rect().xHi, 22)
        self.assertEqual(self.layout.rect(2, fourthIdx).rect().yHi, 23)
    def insert_text_test(self):
        firstText = magicalFlow.TextLayout()
        firstText.text = "first"
        firstText.coord().x = 5
        firstText.coord().y = 6
        firstIdx = self.layout.insertText(0, firstText)
        self.assertEqual(firstIdx, 0)
        firstText = self.layout.text(0, firstIdx)
        self.assertEqual(firstText.text, "first")
        self.assertEqual(firstText.coord().x, 5)
        self.assertEqual(firstText.coord().y, 6)
        firstText.text = "newfirst"
        self.assertEqual(self.layout.text(0, firstIdx).text, "newfirst")

        coord = magicalFlow.XYLoc()
        coord.x = 15
        coord.y = 26
        secondIdx = self.layout.insertText(0, "second", coord)
        self.assertEqual(secondIdx, 1)
        self.assertEqual(self.layout.text(0, secondIdx).text, "second")
        self.assertEqual(self.layout.text(0, secondIdx).coord().x, 15)
        self.assertEqual(self.layout.text(0, secondIdx).coord().y, 26)

        thirdIdx = self.layout.insertText(2, "third", 31, 84)
        self.assertEqual(thirdIdx, 0)
        self.assertEqual(self.layout.text(2, thirdIdx).text, "third")
        self.assertEqual(self.layout.text(2, thirdIdx).coord().x, 31)
        self.assertEqual(self.layout.text(2, thirdIdx).coord().y, 84)
        

if __name__ == '__main__':
    unittest.main()
