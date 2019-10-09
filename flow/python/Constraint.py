#
# @file PnR.py
# @author Mingjie Liu
# @date 09/30/2019
# @brief The class for generating symmetry constraints for PnR
#
import subprocess
import magicalFlow
import S3DET

class Constraint(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
        self.s3det = S3DET.S3DET(self.mDB)

    def genConstraint(self, cktIdx, dirName):
        cktname = self.dDB.subCkt(cktIdx).name
        if self.primaryCell(cktIdx):
            self.primarySym(cktIdx, dirName)
            print "%s is a primary cell, generating constraints." % cktname
            print "Constraints saved at %s.sym" % cktname 
        else:
            self.s3det.systemSym(cktIdx, dirName)
            print "%s is not a primary cell." % cktname
            print "Constraints saved at %s.sym" % cktname 
            #print "Waiving constraints for now."

    def primaryCell(self, cktIdx):
        """
        @brief Checking if cell is primary
        """
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            instNode = ckt.node(nodeIdx)
            subckt = self.dDB.subCkt(instNode.graphIdx)
            if not magicalFlow.isImplTypeDevice(subckt.implType):
                return False
        return True

    def primarySym(self, cktIdx, dirName):
        """
        @brief Generating constraint files with .iniObj
        """
        self.writeInitObj(cktIdx, dirName)
        cirname = self.dDB.subCkt(cktIdx).name
        cmd = "source /home/unga/jayliu/projects/develop/magical/magical/install/constraint_generation/test/run_init.sh " + cirname + " " + dirName
        subprocess.call(cmd, shell=True)

    def writeInitObj(self, cktIdx, dirName):
        """
        @brief write .initObj file
        """
        ckt = self.dDB.subCkt(cktIdx)
        phyDB = self.dDB.phyPropDB()
        filename = dirName + ckt.name + '.initObj'
        fout = open(filename, "w")
        instId = 0
        for nodeIdx in range(ckt.numNodes()):
            instNode = ckt.node(nodeIdx)
            assert not magicalFlow.isImplTypeDevice(instNode.implType), "Circuit %s contains non instance %s" %(ckt.name + instNode.name)
            cirNode = self.dDB.subCkt(instNode.graphIdx)
            instPIdx = cirNode.implIdx
            fout.write("Inst\n%d\n" % instId)
            if cirNode.implType == magicalFlow.ImplTypePCELL_Nch:
                fout.write("NMOS\n%s\n" % instNode.name)
                nch = phyDB.nch(instPIdx)
                fout.write("%de-12 %de-12 %d\n" %(nch.width, nch.length, nch.numFingers))
            elif cirNode.implType == magicalFlow.ImplTypePCELL_Pch:
                fout.write("PMOS\n%s\n" % instNode.name)
                pch = phyDB.pch(instPIdx)
                fout.write("%de-12 %de-12 %d\n" %(pch.width, pch.length, pch.numFingers))
            elif cirNode.implType == magicalFlow.ImplTypePCELL_Res:
                fout.write("RES\n%s\n" % instNode.name)
                res = phyDB.resistor(instPIdx)
                fout.write("%de-12 %de-12\n" %(res.wr, res.lr))
            elif cirNode.implType == magicalFlow.ImplTypePCELL_Cap:
                fout.write("CAP\n%s\n" % instNode.name)
                cap = phyDB.capacitor(instPIdx)
                fout.write("%de-12 %de-12\n" %(cap.w, cap.lr))
            else:
                fout.write("OTHER\n%s\n" % instNode.name)
            for pin_idx in range(instNode.numPins()):
                pinIdx = instNode.pinIdx(pin_idx)
                netIdx = ckt.pin(pinIdx).netIdx
                fout.write("%d\n" % netIdx)
                #fout.write("%d %s\n" %(netIdx,ckt.net(netIdx).name))
            instId += 1
        for net in range(ckt.numNets()):
            fout.write("NET\n%d\n%s\n" % (net, ckt.net(net).name))

