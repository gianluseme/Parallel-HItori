# Hitori Puzzle Solver

This project implements two parallel approaches to solve the Hitori puzzle using the MPI library in C.

## Implemented Approaches

1. **Parallel Depth-First Search (DFS) with Aggressive Pruning**:
    - Uses a parallel depth-first search (DFS) scheme with dynamic load balancing and  pruning techniques to reduce the search space.
    - More computationally efficient by eliminating invalid configurations early.

2. **Parallel Brute Force**:
    - Explores all possible configurations of the \(n \times n\) grid in parallel.
    - Simple and straightforward implementation, highly scalable, suitable for use with a large number of processes.

## Requirements

- MPI (Message Passing Interface)
- A C compiler compatible with MPI (e.g., `mpicc`)
- CMake (v3.23+) for building the project

## Compilation

To compile the program, run the provided `compile.sh` script:

```sh
./compile.sh
```

## Execution

To execute the program with Parallel DFS, run:

```sh
mpirun -np <number_of_processes> ./parallel_hitori -n <grid_size> -c <stack cutoff> -w <work chunk size>
```

To run the program with Parallel Brute Force, run the command:

```sh
mpirun -np <number_of_processes> ./parallel_hitori -n <grid_size> -f
```

By adding the option -b, the program runs in benchmark mode.


## References

- [Parallel Depth First Search](http://users.atw.hu/parallelcomp/ch11lev1sec4.html)
- V. K. V. Nageshwara Rao, "Parallel Depth First Search, Part I: Implementation,", 1987.
