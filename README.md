# MAGICAL #

MAGICAL: Machine Generated Analog IC Layout

This project is currently still under active development.

# Dependency #

- Python 2.7 and additional packages
    - [PyParsing](https://github.com/pyparsing/pyparsing)
    - [networkx](https://github.com/networkx/networkx)

- [Boost](https://www.boost.org)
    - Need to install and visible for linking

- [Limbo](https://github.com/limbo018/Limbo)
    - Integrated as a thirdparty package

- ~~[Gurobi 8.0.1](http://www.gurobi.com/)~~
    - ~~ Replaced with LPSolve~~

- [LPSolve 5.5](http://lpsolve.sourceforge.net/5.5/)
    - Integrated as a thirdparty package 

- [Lemon 1.3.1](https://lemon.cs.elte.hu/trac/lemon)
    - Integrated as a thirdparty package

- [wnlib](http://www.willnaylor.com/wnlib.html)
    - Integrated as a thirdparty package

# How to clone #

To clone the repository and submodules, go to the path to download the repository. 
```
# clone the repository 
git clone https://github.com/magical-eda/MAGICAL.git
```

# How to build #

To build, run make in the root directory. 
```
make 
make install
```
GCC 4.8 or later is preferred. 
Export CC and CXX environment variables for custom gcc and g++ path, respectively. 

To clean, run make clean in the cpp directory. 
```
make clean
```

# How to run #

A toy example of technology input file is under execution/inputs

Benchmark circuit examples are under execution/results

All technology related parameters including benchmark circuit sizing are samples and not related to any proprietary PDK information.

Benchmark circuits currently includes:
1 Comparator
3 OTA

To run the example benchmark circuits
```
cd execution
source run.sh
```
Prior running, please ensure that the correct netlist file is given: /execution/results/circuit_example/circuit_example.sp

The output layout gdsii files: execution/results/circuit_example/circuit_example.route.gds

# Custom layout constraint inputs #

The automatic symmetry constraint generation is currently embedded into the flow. To ensure circuit functionality it is ideal that designers provide  constraints to guide the placement and routing. 

A sample device and net symmetry constraint is given for OTA_1. These files should also be the output for the current automatic symmetry constraint generation flow.

Sample symmetry device constraint file:
execution/results/OTA_1/OTA_1.sym

Sample symmetry net constraint file:
execution/results/OTA_1/OTA_1.symnet

## Device symmetry constraints

Device symmetry constraints greatly affect the placement solution and output layout quality. Currently we only consider symmetry groups, symmetry device pairs and self-symmetric device constraints.

**Symmetry group**: A group of symmetry device pairs and self-symmetric devices that share the same symmetry axis.

**Symmetry device pair**: Two devices that are reflection symmetric with respect to a symmetry axis (usually vertical).

**Self-symmetry device**: A single device that is reflection symmetric with itself respect to a symmetry axis.

For device symmetry constraint input, each line indicate a symmetry device pair or a self-symmetry device constraint. Symmetry groups are seperated by a blank line. The below example of OTA_1.sym shows 3 symmetry group constraints. For the first symmetry group, it consists of 3 symmetry pairs and 2 self-symmetry device sharing the same symmetry axis.
```
M15 M17
M18 M19
M13 M12
MP1c
M20

M2 M0
M3 M1
M6 M9
C0 C1
M23 M25
MP1a MP1b
M11
M10

M5 M7

```
Note: Currently the placement engine strictly follow the constraints. Please be careful that the given constraints could be satisfied. Infeasible constraint inputs would cause MAGICAL to crash.

## Net symmetry constraints

Similar to device symmetry constraints, we consider symmetry net pairs and self-symmetry net constraints. Each line indicate a symmetry net pair or a self-symmetry net constraint. Invalid net symmetry constraints would automatically be waived by the routing engine. Below is the example of OTA_1.symnet:
```
intm intp
INP INM
OUTP OUTM
net047 net037
net020
net028
net7
vcmfb
net025
VREF
```

## Running the flow with custom layout constraints
To run the flow with custom layout constraints, we need to disable the automatic constraint generation since it would overwrite the input constraint files. Comment line 39 and 40 for  constraint_generation/test/run.sh:
```
# echo -e "${STATUS_PREFIX_COLOR}${CIRCUIT_NAME}:${NC} Generating constraints for placement and routing"
# $SFA_DIR/bin/SFA ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME}.initObj ${RESULT_DIR}/${CIRCUIT_NAME}/${CIRCUIT_NAME} > /dev/null
```

Return to the root directory. Re-install and run the script:
```
make install
cd execution
source run.sh
```
