import IdeaPlaceExPy
import sys

def runPlace(cktname, dirname):
    placer = IdeaPlaceExPy.IdeaPlaceEx()
    placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')

    placeParsePin(placer, dirname+cktname+'.pin')
    placeConnection(placer, dirname+cktname+'.con')

    placer.readSymFile(dirname + cktname + '.sym')

    placeParseBoundary(placer, dirname+cktname+'.bound')
    symAxis = placer.solve(200)

def read_tokens(f):
    for line in f:
        for token in line.split():
            yield token

def placeParsePin(placer, fileName):
    inFile = open(fileName, 'r')
    tokens = read_tokens(inFile)
    for nodeIdx in range(int(tokens.next())):
        cellIdx = placer.allocateCell()
        assert nodeIdx == cellIdx
        name = tokens.next()
        placer.setCellName(nodeIdx, name)
        for netIdx in range(int(tokens.next())):
            x = int(tokens.next())
            if x != -1:
                pinIdx = placer.allocatePin(nodeIdx)
                assert pinIdx == placer.pinIdx(nodeIdx, netIdx)
                placer.addPinShape(pinIdx, x,int(tokens.next()),int(tokens.next()),int(tokens.next()))

def placeConnection(placer, fileName):
    inFile = open(fileName, 'r')
    tokens = read_tokens(inFile)
    for netIdx in range(int(tokens.next())):
        dbNetIdx = placer.allocateNet()
        assert netIdx == dbNetIdx
        for pinIdx in range(int(tokens.next())):
            valid = int(tokens.next())
            if valid != -1:
                placer.addPinToNet(valid, netIdx)

def placeParseBoundary(placer, fileName):
    inFile = open(fileName, 'r')
    tokens = read_tokens(inFile)
    for nodeIdx in range(int(tokens.next())):
        placer.addCellShape(nodeIdx, 0, int(tokens.next()),int(tokens.next()),int(tokens.next()),int(tokens.next()))
        
def main(argv):
    runPlace(argv[0], "./")
    
if __name__ == "__main__":
    main(sys.argv[1:])
