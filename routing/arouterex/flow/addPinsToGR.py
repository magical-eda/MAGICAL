import sys

class Polygon():
    def __init__(self):
        self.xArray = []
        self.yArray = []
        self.layer = 1
    def toStr(self):
        string = str(self.layer) + " "
        for idx in range(0, len(self.xArray)):
            string += str(self.xArray[idx]) + " " + str(self.yArray[idx]) + " "
        string += "\n"
        return string


def readGuardRingFile(filename):
    """ Read in the file with the coorinate of a polygon as the shape of guard ring"""
    polys = []
    with open(filename) as fin:
        lines = fin.readlines()
        for line in lines:
            splited = line.split()
            if len(splited) > 0:
                poly = Polygon()
                isX = True
                poly.layer = int(splited[0])
                for idx in range(1, len(splited)):
                    pt = splited[idx]
                    if (isX):
                        isX = False
                        poly.xArray.append(int(pt))
                    else:
                        isX = True
                        poly.yArray.append(int(pt))
                assert len(poly.xArray) == len(poly.yArray), "The number of x and y coordinates are not equal!"
                polys.append(poly)
    return polys

def readGR(infile, outfile, token, polys):
    """ Read in the .gr file and write to the output .gr with the guard ring read before """
    with open(infile, 'r') as fin:
        with open(outfile,'w' ) as fout:
            for line in fin:
                splited = line.split()
                if (len(splited) > 0):
                    if (splited[0] in token):
                        """ This line is the ground and need to add some thing """
                        newLine = str(splited[0]) + " " + str(splited[1]) + " "+ str(int(splited[2]) + len(polys)) + " " + str(splited[3]) + "\n"
                        fout.write(newLine)
                        """ Write the polygon """
                        for poly in polys:
                            fout.write(poly.toStr())

                    else:
                        """ Directly write the line out """
                        fout.write(line)



def main(argv):
    assert len(argv) >= 4, "The number of arguments is too small!"
    if len(argv) >= 5:
        if (argv[4] == "GND" or argv[4] == "VSS" or argv[4] == "gnd" or argv[4] == "vss"):
            token = ["GND", "VSS", "gnd", "vss", "vssa", "VSSA", "Vssa"]
        if (argv[4] == "VDD" or argv[4] == "vdd"):
            token = ["VDD", "vdd", "VDDA", "vdda", "Vdda"]
    else:
        token = ["GND", "VSS"]
    polys = readGuardRingFile(argv[1])
    readGR(argv[2], argv[3], token, polys)

"""
first argv: the input contact pin coordinates file
second argv: the input .gr file
third argv: the output name for .gr file
fourth argv: the token for the net that need to connect to the guard ring (default: GND and VSS)
"""
main(sys.argv) # Invoke the main
