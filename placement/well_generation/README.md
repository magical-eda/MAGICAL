# WELL generation 

Generate WELLs with guidance from GAN 

# Dependency 

- Tensorflow 1.5.1 

- OpenCV-Python 3.4.4

- Python 2.7

- [Boost](www.boost.org)
    - Need to install and visible for linking

- [Limbo](https://github.com/limbo018/Limbo)
    - Integrated as a git submodule

- [Gurobi 8.0.1](http://www.gurobi.com/)
    - Will replace with lpsolve

# How to Build 

To build, run make in the cpp directory. 
```
make 
```
GCC 4.8 or later is preferred. 
Export CC and CXX environment variables for custom gcc and g++ path, respectively. 

To clean, run make clean in the cpp directory. 
```
make clean
```

# How to Run

Run with an input GDSII for a design  
```
source test/run.sh circuit.gds
```
