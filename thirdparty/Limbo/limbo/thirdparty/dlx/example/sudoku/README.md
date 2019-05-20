Sudoku
======

If something is worth doing, it's worth doing well.

The [solver](SudokuSolver.cpp) itself is quite simple. The exact cover problem
has four types of columns, n\*n of each type. (For standard Sudoku, n = 9.)

  - Cell (*x*, *y*) needs to contain a digit.
  - Column *x* needs to contain digit *d*.
  - Row *y* needs to contain digit *d*.
  - Region *i* needs to contain digit *d*.

The first one can actually be either a secondary or a primary column; if all
other conditions are met, every cell will naturally contain *at most one*
digit.

Each row of the matrix hits exactly one column of each type.

For the standard 9x9 Sudoku, all regions are 3x3 squares. But it is
straight-forward to generalize this so that the regions can form an arbitrary
partition of the grid to subsets of size n. This also makes it possible to
create Sudokus of any size; with square regions, the closest alternatives to
9x9 are 4x4 and 16x16.

Usage
=====

The executable tries to infer as much as possible from the input data,
requiring no flags or meta data about the Sudoku types. It expects to find
Sudokus separated by empty lines. Empty cells can be either dots (`'.'`) or
zeros (`'0'`). All non-zero digits and letters can be used as labels.
Regions can be drawn with any non-space non-label characters.

The given Sudokus will be solved, unless they are invalid or unsolvable; in
that case, an error message is printed. A completely empty Sudoku will be
replaced with a randomly generated (and usually quite difficult) one, which is
also solved.

Examples
========

<big><pre>
./build/sudoku < [data/sudoku.txt](/data/sudoku.txt)
./build/sudoku -s -l -f oneline < [sudoku17](http://staffhome.ecm.uwa.edu.au/~00013890/sudoku17) > sudoku17\_solutions
yes . | head -n81 | ./build/sudoku -f default
</pre><big>

TODO
====

  - Use proper exception types and messages.
  - Generate random SudokuTypes.
  - A format that uses / and \ when appropriate?
