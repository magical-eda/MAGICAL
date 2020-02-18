import IdeaPlaceExPy

dirname = './'
cktname = 'OTA_XT_MAGICAL'
placer = IdeaPlaceExPy.IdeaPlaceEx()
placer.readTechSimpleFile('/home/unga/jayliu/projects/inputs/techfile.simple')
placer.readPinFile(dirname + cktname + '.pin')
placer.readConnectionFile(dirname + cktname + '.connection')
placer.readSymFile(dirname + cktname + '.sym')
#placer.initCells()
for nodeIdx in range(placer.numCells()):
    name = placer.cellName(nodeIdx)
    gdsFile = dirname+'gds/'+name+'.gds'
    placer.readGdsLayout(gdsFile, nodeIdx)
    print name, placer.cellName(nodeIdx), nodeIdx
placer.solve()
for nodeIdx in range(placer.numCells()):
    x_offset = placer.xCellLoc(nodeIdx)
    y_offset = placer.yCellLoc(nodeIdx)
    print placer.cellName(nodeIdx), x_offset, y_offset
