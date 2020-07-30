##
# @file StdCell.py
# @date 10/23/2019
# @brief The class for dealing with standard cells
#

import magicalFlow
import Router
import subprocess

class StdCell(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB

    def setup(self, cktIdx, dirName):
        ckt = self.dDB.subCkt(cktIdx)
        Router.Router(self.mDB).readBackDumbFile(dirName+'stdcell/'+ckt.name+'.route.gds.dumb', cktIdx)
        #cmd = "cp " + dirName+'stdcell/'+cirname+'.route.gds ' + dirName+cirname+'.route.gds'
        #subprocess.call(cmd, shell=True)
        self.dDB.subCkt(cktIdx).isImpl = True
        # Read standard cell.
        ckt.setTechDB(self.tDB)
        ckt.parseGDS(dirName+'stdcell/'+ckt.name+'.route.gds')
