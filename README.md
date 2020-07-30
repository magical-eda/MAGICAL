# magical

MAGICAL: Machine Generated Analog IC Layout

There is the top-level MAGICAL flow repository. In MAGICAL, we maintain seperate components, such as constraint generation, placement and routing, in different repository. And we integrate each component through top-level python flow.

More information, instructions and benchmark circuits will be added later.

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
```
GCC 4.8 or later is preferred. 
Export CC and CXX environment variables for custom gcc and g++ path, respectively. 

To clean, run make clean in the cpp directory. 
```
make clean
```

# How to Run

Require a set of input technology files and a design.  
```
source test/run.sh circuit techfile simple_techfile spacing_rule width_area_rule enclosure_rule well_contact_GDSII lef_file
```

circuit: a directory to a folder CIRCUIT_NAME containing CIRCUIT_NAME.sp Spectre netlist. 

techfile: technology file from PDK. 

simple_techfile: simplified technology file. 

spacing_rule: spacing rule file. 

width_area_rule: minimum width and area rule file. 

enclosure_rule: enclosure rule file. 

well_contact_GDSII: a GDSII file defining WELL contacts. 

lef_file: LEF file. 
