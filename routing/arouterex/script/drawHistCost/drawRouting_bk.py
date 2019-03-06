import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm

xArray,yArray,raw = np.loadtxt('../../bin/debug/his.txt').T

numRow = int(xArray.max() + 1)
numCol = int(yArray.max() + 1)

grid = np.zeros((numRow, numCol))

rawIdx = 0
for x in range(0, numRow):
    for y in range(0, numCol):
        grid[x][y] = raw[rawIdx]
        if (np.isclose(raw[rawIdx], 0)):
            grid[x][y] = np.nan
        rawIdx += 1

plt.imshow(grid.T, interpolation='nearest', cmap=cm.gist_rainbow_r)
plt.colorbar()
plt.savefig("draw.pdf")
