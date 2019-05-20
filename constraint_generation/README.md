# CONSTRAINT generation 

Generate Symmetry Constraints for analog placement and routing

# Dependency 

- Python 2.7

- [pyparsing](www.pypi.org/project/pyparsing)
    - Need to install and visible for linking

- [networkx](www.networkx.github.io)
    - Need to install and visible for linking

# How to Build 

To build, run the following command
```
cd signal_flow_analysis
mkdir build 
cd build
cmake ..
make 
```
GCC 4.8 or later is preferred. 
Export CC and CXX environment variables for custom gcc and g++ path, respectively. 

To clean, run make clean in the build directory. 
```
cd signal_flow_analysis/build
make clean
```

# How to Run

Run with an input spectre netlist for a design  
```
source test/run.sh circuit.sp
```
