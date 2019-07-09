##
# @file Device_generator.py
# @date 07/07/2019
# @brief The class for generating the layout for pcell devices
#

class Device_generator(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
    def generateDevice(self, cktIdx):
        return True
