Modified from https://github.com/jlaire/dlx-cpp to be C++98 compatible 

Dancing Links
=============

This is an efficient C++ implementation of Algorithm DLX from Knuth's *Dancing
Links* paper for the exact cover problem.

Knuth's paper gives a very readable explanation of the problem, the algorithm,
and several applications. I think it's still the best place to start.
http://arxiv.org/pdf/cs/0011047v1.pdf

Implementation
==============

The implementation consists of a few classes defined under
[include/dlx/](include/dlx/) and [src/](src/). There is no documentation at the
moment, so take a look at the examples.

`dlx` can also solve generalized exact cover problems (see Knuth's paper). The
columns of the matrix should be sorted so that all secondary columns are on the
left, before primary columns. N-queens is a good example of this.

Example: dlx
============

[example/dlx](example/dlx) is a simple command-line program that reads an exact
cover problem from stdin and solves it.

The first line of stdin contains the number of columns, and the following lines
contain the rows of the matrix.

Output can be controlled by flags. By default, only the number of solutions is
printed. If `-p` is given, every solution is printed on its own line by giving
the indices of the selected rows. With `-v`, the full rows are printed.

<big><pre>
$ make examples
$ ./build/dlx -pv < [data/knuth\_example.txt](data/knuth_example.txt)
1 0 0 1 0 0 0
0 0 1 0 1 1 0
0 1 0 0 0 0 1<br><br>solutions: 1
</pre></big>

With `-s`, input can be given as a sparse matrix.

<big><pre>
$ ./build/dlx -ps < [data/knuth\_example\_sparse.txt](data/knuth_example_sparse.txt)
3 0 4
solutions: 1
</pre></big>

To solve a generalized exact cover problem, put the number of secondary columns
on the first line, after the number of all columns. The default value is zero,
in other words, a regular exact cover problem.

<big><pre>
$ ./build/dlx -pv < [data/generalized\_example.txt](data/generalized_example.txt)
0 1 1<br><br>solutions: 1
</pre></big>

Example: Sudoku
===============

This program can solve and generate various types of Sudokus. See
[example/sudoku/README.md](example/sudoku/README.md) for details.

<big><pre>
$ make examples
$ ./build/sudoku < [data/sudoku.txt](data/sudoku.txt)
[output](https://gist.github.com/jlaire/4429d93d016b73ef6511)
</pre></big>

Example: N-queens
=================

Place N queens on an NxN chessboard so that they don't attack each other. This
is a good example of a generalized exact cover problem: each diagonal must
contain *at most* one queen, but zero is ok.

```
$ make examples
$ ./build/nqueens 8 12
Solutions for n=8: 92
Solutions for n=12: 14200
```

```
$ ./build/nqueens -v 1 2 3 4
Solutions for n=1: 1
Q

Solutions for n=2: 0
Solutions for n=3: 0
Solutions for n=4: 2
..Q.
Q...
...Q
.Q..

.Q..
...Q
Q...
..Q.
```

Example: Langford pairings
==========================

See [Wikipedia](https://en.wikipedia.org/wiki/Langford_pairing).

    $ make examples
    $ ./build/langford -v 1 2 3 4 5
    Solutions for n = 1: 0
    Solutions for n = 2: 0
    Solutions for n = 3: 1
    3 1 2 1 3 2
    Solutions for n = 4: 1
    4 1 3 1 2 4 3 2
    Solutions for n = 5: 0

Example: N-pieces
=================

Generalized version of N-queens: place N knights and Q queens on an AxB board.
Quite slow, unfortunately.

    $ make examples
    $ ./build/npieces 8 8 5 5
    Solutions for 8x8, N=5, Q=5: 16
    NN......
    ....Q...
    ......Q.
    NN......
    N.......
    .......Q
    .....Q..
    ..Q.....
    <snip>

Example: Polyominoes
====================

The code can solve any polyomino puzzle, but for now the executable simply
prints all solutions to Scott's pentomino problem:

    $ make examples
    $ ./build/polyomino | head -n8
    LLXCCVVV
    LXXXCVZZ
    LNXCCVZY
    LNT  ZZY
    NNT  WYY
    NTTTWWFY
    PPPWWFFF
    PPIIIIIF

TODO
====

  - ExactCoverProblem: Export in Knuth's format for benchmarking.
  - AlgorithmDLX: Option to estimate the search tree with Monte Carlo.
  - All examples: Option to export the exact cover problem. A hacky solution
    would be an environment variable that enables this within the dlx library
    itself.
  - CMake
  - Polyomino: Find unique solutions.
  - More examples:
    * Latin squares (Knuth, volume 4a)
