# PixBalance

Let's write a function, where the input is a m x n matrix of integer values in the set {0, 1, 2, 3, 4}.
For each cell with values 0 and 1, we need to replace it by the most frequent value in its 3x3 neighborhood. The neighborhood does not include the cell itself. If there is a tie, we should choose the smallest value. The process is repeated until no cell is updated. We should consider periodic boundary conditions, i.e., the matrix is a torus.

Requisites:
- The code should be parallelized using OpenMP.
- The code should be well-documented.
- The code should be tested with a matrix of size 10x10 with random values.
