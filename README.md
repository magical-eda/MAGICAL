# magical

MAGICAL: Machine Generated Analog IC Layout

# Dependency 

- Python 2.7

- [Boost](www.boost.org)
    - Need to install and visible for linking

- [Limbo](https://github.com/limbo018/Limbo)
    - Integrated as a git submodule

- [Gurobi 8.0.1](http://www.gurobi.com/)
    - Will replace with LPSolve

- [LPSolve 5.5](http://lpsolve.sourceforge.net/5.5/)
    - Integrated as a thirdparty package 

- [Lemon 1.3.1](https://lemon.cs.elte.hu/trac/lemon)
    - Integrated as a thirdparty package

- [wnlib](http://www.willnaylor.com/wnlib.html)
    - Integrated as a thirdparty package

# How to clone 

To clone the repository and submodules, go to the path to download the repository. 
```
# clone the repository 
git clone git@bitbucket.org:limbo018/magical.git 
# initialize submodules
git submodule init 
# update submodules 
git submodule update
```

# How to Build 

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

# How to Run

A toy example of technology input file is under execution/inputs

Benchmark circuit examples are under execution/results

All technology related parameters including benchmark circuit sizing are samples and not related to any real proprietary PDK information.

Benchmark circuits currently includes:
1 Comparator
3 OTA

To run the example benchmark circuits
```
cd execution
source run.sh
```

The output layout gdsii files are under execution/results/circuit_example/circuit_example.route.gds
