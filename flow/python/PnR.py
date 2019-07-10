##
# @file PnR.py
# @author Keren Zhu
# @date 07/07/2019
# @brief The class for implementing one layout for a circuit
#


class PnR(object):
    def __init__(self, magicalDB):
        self.mDB = magicalDB
        self.dDB = magicalDB.designDB.db
        self.tDB = magicalDB.techDB
    
    def implLayout(self, cktIdx, dirname):
        """
        @brief PnR a circuit in the designDB
        @param the index of subckt
        """
        return True
    def writeBlockFile(self, cktIdx, filename):
        """
        @brief write .block file
        """
        ckt = self.dDB.subCkt(cktIdx)
        fout = open(filename, "w")
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write(" ")
            fout.write(str(subckt.gdsData().bbox().xLen()))
            fout.write(" ")
            fout.write(str(subckt.gdsData().bbox().yLen()))
            fout.write("\n")
    def writeConnectionFile(self, cktIdx, filename):
        """
        @brief write .connection file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            fout.write(net.name)
            fout.write(" ")
            for pinIdx in net.numPins():
                pin = ckt.pin(pinIdx)
                nodeIdx = pin.nodeIdx()
                node = ckt.node(nodeIdx)
                fout.write(node.name)
                fout.write(" ")
                fout.write(str(pin.intNetIdx))
                fout.write(" ")
            fout.write("\n")
    def writeNetFile(self, cktIdx, filename):
        """
        @brief write .net file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for netIdx in range(ckt.numNets()):
            net = ckt.net(netIdx)
            fout.write(net.name)
            fout.write(" ")
            for pinIdx in net.numPins():
                pin = ckt.pin(pinIdx)
                nodeIdx = pin.nodeIdx()
                node = ckt.node(nodeIdx)
                fout.write(node.name)
                fout.write(" 0 0 ")
            fout.write("\n")
    def writeOffsetFile(self, cktIdx, filename):
        """
        @brief write .offset file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write(" ")
            xLo = subCkt.gdsData().bbox().xLo
            yLo = subCkt.gdsData().bbox().yLo
            fout.write(str(float(xLo) / 1000))
            fout.write(" ")
            fout.write(str(float(yLo) / 1000))
            fout.write("\n")
    def writePinFile(self, cktIdx, filename):
        """
        @brief write .pin file
        """
        fout = open(filename, "w")
        ckt = self.dDB.subCkt(cktIdx)
        for nodeIdx in range(ckt.numNodes()):
            node = ckt.node(nodeIdx)
            subCkt = self.dDB.subCkt(node.graphIdx)
            fout.write(node.name)
            fout.write("\n")
            offsetXLo = float(subCkt.gdsData().bbox().xLo) / 1000
            offsetYLo = float(subCkt.gdsData().bbox().yLo) / 1000
            for netIdx in range(subCkt.numNets()):
                net = subCkt.net(netIdx)
                shape = net.ioShape()
                layer = net.ioLayer()
                xLo = float(shape.xLo / 1000) 
                yLo = float(shape.yLo / 1000) 
                xHi = float(shape.xHi / 1000) 
                yHi = float(shape.yHi / 1000) 
                fout.write(str(netIdx))
                fout.write("    1\n ")
                fout.write("M")
                fout.write(str(layer))
                fout.write("    ((")
                fout.write(str(xLo - offsetXLo))
                fout.write(" ")
                fout.write(str(yLo - offsetYLo))
                fout.write(") (")
                fout.write(str(xHi - offsetXLo))
                fout.write(" ")
                fout.write(str(yHi - offsetYLo))
                fout.write("))\n")



            
