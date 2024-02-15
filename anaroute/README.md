# anaroute

The MAGICAL analog router.

# How to install ?

The following dependencies are required.

 - [Boost](www.boost.org) >= version 1.6
 - [LIMBO](https://github.com/limbo018/Limbo) Set the env variable LIMBO\_DIR or specify `cmake -DLIMBO_DIR=<path>`
 - [lemon](https://lemon.cs.elte.hu) Set the env variable LEMON\_DIR or specify `cmake -DLEMON_DIR=<path>`
 - [SparseHash](https://github.com/sparsehash/sparsehash) Set the env variable SPARSE\_HASH\_DIR or specify `cmake -DSPARSE\_HASH\_DIR=<path>`
 - [pybind11](https://github.com/pybind/pybind11) Please see the official [document](http://pybind11.readthedocs.org/en/master) for installing the pybind11.
    Set the env variable `PYBIND11_DIR` or specify `cmake -DPYBIND11_DIR=<path>`.
    To use other Python versions, add cmake flags `cmake -DPYTHON_EXECUTABLE=<path>`
 
In the directory of anaroute, run
```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=absolute/path/to/your/installation -DPYTHON_EXECUTABLE=<path> ..
make
make install
cp *.so ~/.local/lib/pythonX.X/site-packages/ (your python version)
```

# How to use ?
Binary executable
```
./anaroute --tech_lef=string --tech_file=string --design_file=string --placement_layout=string --out=string [options] ...
options:
      --tech_lef            technology lef file (string)
      --tech_file           technology layer file (string)
      --design_file         design file (string)
      --placement_layout    placement layout file (string)
  -o, --out                 output file (string)
      --symnet              symmetric nets file (string [=])
      --iopin               IO-Pin file (string [=])
      --power               power net file (string [=])
      --spec                net spec file (string [=])
      --fuck                fuck file (string [=])
      --out_guide           output global routing guide file (string [=])
      --flatten             flatten output GDS
  -?, --help                print this message

```

Python Interface
```
  parseLef: parse LEF file (TSMC40)
  parseTechFile: parse techfile (TSMC40)
  parseGds: parse placement layout
  parseSymNet: set symnets from file
  parsePower: set power nets from file
  parseIOPin: set IO ports from file
  setCirBBox: set the circuit BBox 
  setGridStep: set routing pitch size
  setGridOffsetX: set detailed routing grid origin (x-coordinate)
  setGridOffsetY: set detailed routing grid origin (y-coordinate)
  setSymAxisX: set default symmetric axis
  addNet: add a net
  addPin: add a pin
  addPin2Net: add a pin to a net
  addShape2Pin: add a physical shape to a pin
  addBlk: add a blockage
  addSymNet: add a symmetry net
  addSelfSymNet: add a self-symmetry net
  addIOPort: add an IO port
  solve: solve routing
  evaluate: compute routing statistics
  writeLayoutGds: output the final routed layout
```
