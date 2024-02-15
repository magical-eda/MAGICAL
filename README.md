# Setup Guide #

Open this repository in codespaces. .devcontainer contains configurations to automatically build the docker container.

When adding new features, create a new branch and make a pull request.

Launch.json contains some debug setups. If you want to run the debugger, just press Ctrl+P or the top search bar, and type "debug COMP/NOT/ADC1" etc

## Weird Things ##

If python can't find the magicalflow library, make sure you're using the below as your python install
`/usr/local/bin/python3`

If you're running the debugger for the first time, you might have some errors about a missing gds folder. Just create a new gds folder underneath the example directory (for example, just create a new gds folder underneath /examples/adc1)

# MAGICAL #

MAGICAL: Machine Generated Analog IC Layout

This is the top-level MAGICAL flow repository. In MAGICAL, we maintain seperate components, such as constraint generation, placement and routing, in different repository. And we integrate each component through top-level python flow.

This project is currently still under active development.

# Dependency #

- Python 3.7 and additional packages
    - Refer to [Dockerfile](https://github.com/magical-eda/MAGICAL/blob/docker/Dockerfile).

- [Boost](https://www.boost.org)
    - Need to install and visible for linking. Required version at least 1.62.

- [Flex](https://github.com/westes/flex)
    - Need to install, required by Limbo package

- [Zlib](https://www.zlib.net)
    - Required by Limbo package

- [Limbo](https://github.com/limbo018/Limbo)
    - Required latest

- [LPSolve 5.5](http://lpsolve.sourceforge.net/5.5/)
    - Required version 5.5

- [Lemon 1.3.1](https://lemon.cs.elte.hu/trac/lemon)
    - Required version 1.3.1


# How to clone #

To clone the repository and submodules, go to the path to download the repository. 
```
# clone the repository 
git clone https://github.com/magical-eda/MAGICAL.git
git submodule init
git submodule update
```

# How to build #

Two options are provided for building: with and without [Docker](https://hub.docker.com). You can also build from source (NOT RECOMMENDED) resolving the required dependancies first.

## Build with Docker

You can use the Docker container to avoid building all the dependencies yourself. 
1. Install Docker on [Linux](https://docs.docker.com/install/).
2. Navigate to the repository. 
    ```
    cd MAGICAL
    ```
3. Get the docker container with either of the following options. 
    - Option 1 (Recommended): pull from the cloud  [jayl940712/magical](https://hub.docker.com/r/jayl940712/magical). 
    ```
    docker pull jayl940712/magical:latest
    ```
    - Option 2: build the container. 
    ```
    docker build . --file Dockerfile --tag magical:latest
    ```
4. Run the docker container
    ```
    docker run -it -v $(pwd):/MAGICAL jayl940712/magical:latest bash
    ```
    Or if you used option 2 to build the container
    ```
    docker run -it -v $(pwd):/MAGICAL magical:latest bash
    ```
# How to run #

Benchmark circuit examples are under examples/

All technology related parameters including benchmark circuit sizing are samples and not related to any proprietary PDK information.

Benchmark circuits currently includes:
1 adc, 1 comparator, 3 ota

To run the benchmark circuits
```
cd /MAGICAL/examples/BENCH/ (ex. adc1)
source run.sh
```

The output layout gdsii files: BENCH/TOP_CIRCUIT.route.gds (ex. adc1/xxx.route.gds)

Note: currently adc2 have routing issues.

# Custom layout constraint inputs #

The automatic symmetry constraint generation is currently embedded into the flow. To ensure circuit functionality it is ideal that designers provide  constraints to guide the placement and routing. 

A sample device and net symmetry constraint is given for adc1. These files should also be the output for the current automatic symmetry constraint generation flow.

Sample symmetry device constraint file:
examples/adc1/CTDSM_TOP.sym

Sample symmetry net constraint file:
examples/adc1/CTDSM_TOP.symnet

## Device symmetry constraints

Device symmetry constraints greatly affect the placement solution and output layout quality. Currently we only consider symmetry groups, symmetry device pairs and self-symmetric device constraints.

**Symmetry group**: A group of symmetry device pairs and self-symmetric devices that share the same symmetry axis.

**Symmetry device pair**: Two devices that are reflection symmetric with respect to a symmetry axis (usually vertical).

**Self-symmetry device**: A single device that is reflection symmetric with itself respect to a symmetry axis.

## Net symmetry constraints

Similar to device symmetry constraints, we consider symmetry net pairs and self-symmetry net constraints. 

**Symmetry net pair**: Two nets that are reflection symmetric with respect to a symmetry axis (usually vertical). For a valid constraint, the corresponding pins of the two nets must be reflective symmetric with a axix.

**Self-symmetry net**: A single net that is reflection symmetric with itself respect to a symmetry axis.

# License #
[BSD 3-Clause](https://github.com/magical-eda/MAGICAL/blob/master/LICENSE)
