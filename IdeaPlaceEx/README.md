IdeaPlaceEx
--------
This is a analog placement engine.
# Install

Some components depend on external libraries, such as 

[Boost](www.boost.org)

[LIMBO](https://github.com/limbo018/Limbo) Configure, make and install the LIMBO package. Set environment variable LIMBO\_DIR to the path, or specify the path to the library as Cmake cache variable LIMBO\_DIR `cmake -DLIMBO_DIR=$PATH`


[lemon](https://lemon.cs.elte.hu) Configure, make and install the Lemon package. Set environment variable LEMON\_DIR to the path, or specify the path to the library as Cmake cache variable LEMON\_DIR `cmake -DLEMON_DIR=$PATH`

[Eigen](http://eigen.tuxfamily.org/index.php) Configure, make and install the Eigen package. Set environment variable EIGEN\_DIR to the path, or specify the path to the library as Cmake cache variable EIGEN\_DIR `cmake -DEIGEN_DIR=$PATH`

[pybind11](https://github.com/pybind/pybind11) Please see the official [document](http://pybind11.readthedocs.org/en/master) for installing the pybind11.
Set the path to pybind11. Either export to system variable `PYBIND11_DIR` or add flag to cmake `cmake -DPYBIND11_DIR=<path>`.
The cmake will automatically find the system Python.
To use the other Python, add cmake flags `cmake -DPYTHON_INCLUDE_DIR=<path> -DPYTHON_LIBRARIES=<static libarary>`.
For example, `-DPYTHON_INCLUDE_DIR=<path>/include/python3.7m -DPYTHON_LIBRARIES=<path>/lib/libpython3.7m.a`

Either Gurobi or lpsolve.

[Gurobi](https://www.gurobi.com) Install the Gurobi package. Set environment variable GUROBI\_HOME to the path, or specify the path to the library as Cmake cache variable GUROBI\_HOME `cmake -DGUROBI_HOME=$PATH`

[lpsolve](http://lpsolve.sourceforge.net) Install the lpsolve package. Set environment variable LPSOLVE\_DIR to the path, or specify the path to the library as Cmake cache variable LPSOLVE\_DIR` `cmake -DLPSOLVE\_DIR=$PATH`
,
Build the program:
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DOCUMENTATION=ON ..
cmake -DCMAKE_BUILD_TYPE=Release  .. (alternative) # If no doxygen
make
make doc (optional)
cd ../../
pip install IdeaPlaceEx
```

Specify the install prefix: 
```
cmake -DCMAKE_INSTALL_PREFIX=./install
```
```
--------
# Usage

Please use the Python interface for this program.
The interface is defined in src/api/IdeaPlaceExAPI.cpp
Usage example can be found in [MAGICAL](https://github.com/magical-eda/MAGICAL)

--------
--------
# Copyright
The software is released under MIT license except third party packages.
