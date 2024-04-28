
# Top K Shortest Path Problem Using MPI and OpenMP
This project implements the Kth Shortest Path Problem using a combination of MPI (Message Passing Interface) for distributed computing and OpenMP for shared memory parallelization. The implementation aims to find the Kth shortest paths among all nodes of a given graph, efficiently utilizing parallel computing resources.

## Prerequisites

Before you begin, ensure you have the following installed on your distibuted systems:
- MPI (e.g., MPICH, Open MPI)
- An OpenMP-compatible compiler (e.g., GCC)

## Installation

Clone the repository to your local machine by clicking [here](https://github.com/anas-farooq8/Top-Kth-Shortest-Path) or run the following commands:

```bash
git clone https://github.com/anas-farooq8/Top-Kth-Shortest-Path
cd Top-Kth-Shortest-Path
```

## Compilation
`mpicc -fopenmp -o parallel parallel.c -lm`

- mpicc: This is the MPI compiler wrapper for C programs.
- -fopenmp: This flag tells the compiler to enable the OpenMP extension.
- -o parallel: This option specifies the output filename of the compiled program.
- -lm: This links the math library, which is required if your program uses mathematical functions provided by this library, such as sin(), cos(), sqrt(), etc.

## Execution
`mpiexec -n 4 -f machinefile ./parallel`

## Report
![Report_page-0001](https://github.com/anas-farooq8/Top-Kth-Shortest-Path/assets/150327092/deef91ad-9c0a-401b-88e3-571676835d3c)
![Report_page-0002](https://github.com/anas-farooq8/Top-Kth-Shortest-Path/assets/150327092/140a71e4-f5b6-43e6-996f-5bc13965e22c)
![Report_page-0003](https://github.com/anas-farooq8/Top-Kth-Shortest-Path/assets/150327092/1db9d57e-c1ab-4ce0-8aea-654e07cbbeae)
![Report_page-0004](https://github.com/anas-farooq8/Top-Kth-Shortest-Path/assets/150327092/546a906d-31f4-4066-bfcb-d793a55964c5)

### Additional Notes:
- Make sure to provide a `machinefile` example or instructions on how to create one if your target users may not be familiar with this setup.
- Adjust the repository URL and any paths or filenames to match your actual project structure.
- It's good practice to also include any special instructions for different environments or additional configurations needed for complex setups.
