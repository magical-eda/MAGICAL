Arouter
--------
# Install

Some components depend on external libraries, such as 

[Boost](www.boost.org)

[LIMBO](https://github.com/limbo018/Limbo) Configure, make and install the LIMBO package. Set environment variable LIMBO\_DIR to the path, or specify the path to the library as Cmake cache variable LIMBO\_DIR `cmake -DLIMBO_DIR=$PATH`


[SparseHash](https://github.com/sparsehash/sparsehash) Configure, make and install the Google Sparse Hash package. Set environment variable SPARSE\_HASH\_DIR to the path, or specify the path to the library as Cmake cache variable SPARSEHASH\_DIR `cmake -DSPARSEHASH_DIR=$PATH`


Build the program:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCUMENTATION=ON ..
cmake -DCMAKE_BUILD_TYPE=Release  .. (alternative) # If no doxygen
make
make doc (optional)
```

Specify the install prefix: 
```
cmake -DCMAKE_INSTALL_PREFIX=./install
```
```
--------
# Usage
The program options:
```
--designType -b:    Optional. The format of input design. Options: "ispd08" "ispd08pin"
--designFile -f:    The input placement/benchmark
--iopin             Optional. The file specifying the IO ports, which are the ones that need to be labeled for the GDSII
--symnet            Optional. The file specifying which pairs of nets need to be routed symmetrically.
--placementLayout   Optional. The placement GDSII layout. The shapes in this layout will be regarded as obstacles in the routing.
--techLef -l:       The technology LEF file
--techfile -t:      The techfile from PDK. Used to get the correct layer ID.
--grAlgo -g:        Optional. The global routing algorithm to be used. Now only has "SequentialAStar"
--drAlgo -g:        Optional. The detailed routing algorithm to be used. Now only has "SequentailAStar"
--log:              Optional. The output log file name
--out:              Optional. The file name for the output GDSII layout file. Default "out.gds"
```

--------
# Example of running the software

A example of the top-level script is provided in `./flow`

Put the compiled binary in `./flow`, and run
```
source run.sh $PATH_TO_DESIGN
```

In `PATH_TO_DESIGN`, 6 inputs are required:

```
$DESIGN.gds
$DESIGN.gr
$DESIGN.iopin
$DESIGN.sub
$DESIGN.symnet
$DESIGN.wcon
```

where `$DESIGN` is the same as the folder name of the input path.

--------
--------
# Copyright
The software is released under MIT license except third party packages.

Third party package **LEF/DEF v5.8 and Above Toolkit** is released under Apache License. [Web](https://projects.si2.org/openeda.si2.org/projects/lefdefnew/)

Third party package **Limbo** is released under MIT License. [GitHub](https://github.com/limbo018/Limbo)

Third party package **Google Test** is released under BSD-3-Clause License. [GitHub](https://github.com/google/googletest)

Third party package **cmdline** is released under BSD-3-Clause License. [Github](https://github.com/tanakh/cmdline)

Third party package **boost** is released under Boost Software License. [Web](https://www.boost.org/)

Third party package **CMake** is released under BSD 3-Clause License. [Web](https://cmake.org/)

Third parth packagea **Google SparseHash** under BSD 3-Clause License. [GitHub](https://github.com/sparsehash/sparsehash) [SourceForge](http://goog-sparsehash.sourceforge.net/)
