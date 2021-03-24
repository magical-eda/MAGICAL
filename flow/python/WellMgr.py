##
# @file WellMgr.py
# @author Keren Zhu
# @date 03/14/2021
# @brief Well generation manager
#

import magicalFlow
from device_generation.basic import basic as basic
import numpy as np
from PIL import Image
from models.WellGAN import pix2pix
import tensorflow.compat.v1 as tf
tf.disable_v2_behavior()

class WellMgr(object):
    def __init__(self, ddb, tdb):
        """
        @param ddb: a magicalFlow.DesignDB object
        """
        self._ddb = ddb
        self._tdb = tdb
        self._util = magicalFlow.DataWellGAN(ddb, tdb.pdkLayerToDb(basic.layer['OD']))
        self.imageSize = 256 # 256 * 256
        self._scale = self._tdb.units().dbu * 0.06 # pixel = 1um / 0.06. Ask WellGAN for this number
    def clear(self):
        self._util.clear()
    def constructCkt(self, cktIdx):
        # Extract well shapes from layouts
        self._util.construct(cktIdx)
        # Scale and convert them in images
        # WellGAN's initial implementation first convert layout into 512*256*3,
        # where the left part is including NW and right part is not include.
        # The left part is basically golden.
        # In GAN model, the data are actually processed as 256 * 256 * 6,
        # where the first three channels are "input" and the last three are "output"
        # There needs some normalization too.
        # In this implementation, we just do everything together
        #
        # We need to crop the whole layouts into smaller clips
        # Each clip is 256 / scale * 256 / scale in db unit (so it's actually technology dependent)
        bbox = self._util.bbox()
        self._xLo = bbox.xLo
        self._yLo = bbox.yLo
        width = self.pixelX(bbox.xHi)
        height = self.pixelY(bbox.yHi)
        self.numRow = int(height / self.imageSize)  + 1 
        self.numCol = int(width / self.imageSize) + 1
        self.imgs = np.zeros(( self.numRow * self.numCol, self.imageSize, self.imageSize, 6), dtype=np.float32)
        def fillRect(rect, channels):
            colBegin, colBeginOffset = self.imgX(rect.xLo)
            colEnd, colEndOffset = self.imgX(rect.xHi)
            rowBegin, rowBeginOffset = self.imgY(rect.yLo)
            rowEnd, rowEndOffset = self.imgY(rect.yHi)
            for row in range(rowBegin, rowEnd + 1):
                rStartIdx = 0
                rEndIdx = self.imageSize - 1
                if row == rowBegin:
                    rStartIdx = rowBeginOffset
                if row == rowEnd:
                    rEndIdx = rowEndOffset
                for col in range(colBegin, colEnd + 1):
                    cStartIdx = 0
                    cEndIdx = self.imageSize - 1
                    if col == colBegin:
                        cStartIdx = colBeginOffset
                    if col == colEnd:
                        cEndIdx = colEndOffset
                    for r in range(rStartIdx, rEndIdx + 1):
                        for c in range(cStartIdx, cEndIdx + 1):
                            for ch in channels:
                                self.imgs[self.imgIdx(row, col)][r][c][ch] = 1.0 

        for odIdx in range(self._util.numPchOdRects()):
            rect = self._util.odPchRect(odIdx)
            fillRect(rect, [0, 3]) #R
        for odIdx  in range(self._util.numOtherOdRects()):
            rect = self._util.odOtherRect(odIdx)
            fillRect(rect, [1, 4]) #G
    def infer(self):
        with tf.Session() as sess:
            model = pix2pix(sess)
            infer = model.test(self.imgs)  / 2.0 + 0.5
            print(infer[7])
            self.inferred=infer[:,:,:,2]
            self.drawInferredImage()

    def imgIdx(self, row, col):
        return row + col * self.numRow
    def pixelX(self, x):
        return int( ( x - self._xLo) / self._scale)
    def pixelY(self, y):
        return int( ( y - self._yLo) / self._scale)
    def imgX(self, x):
        """
        return col, offset
        """
        rx = self.pixelX(x)
        return int(rx / self.imageSize), rx % self.imageSize
    def imgY(self, y):
        """
        return row, offset
        """
        ry = self.pixelY(y)
        return int(ry / self.imageSize), ry % self.imageSize
    def drawInputImage(self):
        """
        Draw self.img, for debugging purpose
        """
        img = self.imgs[7]
        crop = img[:,:, :3] * 255
        img_s = Image.fromarray(crop.astype(np.uint8), 'RGB') # fromarray only works with uint8
        img_s.show()
    def drawInferredImage(self):
        """
        Draw self.img, for debugging purpose
        """
        print(self.inferred.shape)
        input_img = self.imgs[7]
        r,g = input_img[:,:,0], input_img[:,:,1]
        b = self.inferred[7] 
        print("r", r[:,:,np.newaxis].shape)
        img = np.concatenate((r[:,:,np.newaxis],g[:,:, np.newaxis],b[:,:, np.newaxis]), axis=-1)
        print(img.shape)
        print(b * 255)
        crop = (img )* 255
        img_s = Image.fromarray(crop.astype(np.uint8), 'RGB') # fromarray only works with uint8
        img_s.show()
