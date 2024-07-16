#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stddef.h>
#include <getopt.h>
#include "hitoriseqfunctions.h"
#include "hitoriparallelfunctions.h"

MPI_Datatype create_compressed_state_type(void) {
    MPI_Datatype compressed_state_type;
    int blocklengths[3] = {1, 1, 1 };
    MPI_Datatype types[3] = {MPI_UINT64_T, MPI_INT, MPI_INT};
    MPI_Aint offsets[3];

    offsets[0] = offsetof(CompressedState, compressed_status);
    offsets[1] = offsetof(CompressedState, row);
    offsets[2] = offsetof(CompressedState, col);

    MPI_Type_create_struct(3, blocklengths, offsets, types, &compressed_state_type);
    MPI_Type_commit(&compressed_state_type);

    return compressed_state_type;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype compressed_state_type = create_compressed_state_type();

    printf("Process %d: Starting, total processes: %d\n", rank, size);

    int n = 0;
    bool random = true;
    int stack_cutoff = 0;
    int work_chunk_size = 0;
    bool benchmark_mode = false;

    int option;
    while ((option = getopt(argc, argv, "n:pc:w:b")) != -1) {
        switch (option) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'p':
                random = false;
                n = 8;
                break;
            case 'c':
                stack_cutoff = atoi(optarg);
                break;
            case 'w':
                work_chunk_size = atoi(optarg);
                break;
            case 'b':
                benchmark_mode = true;
                break;
            default:
                if (rank == 0) {
                    printf("Usage: %s -n <side> [-p] [-a] -c <cutoff depth> -w <work chunk size>\n", argv[0]);
                }
                MPI_Finalize();
                return 1;
        }
    }

    if (n == 0) {
        if (rank == 0) {
            printf("Please, specify the puzzle size!\n");
        }
        MPI_Finalize();
        return 1;
    }

    if (stack_cutoff == 0 || work_chunk_size == 0) {
        if (rank == 0)
            printf("Please, specify the cutoff depth and work chunk size parameters\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        printf("STACK_CUTOFF: %d\n", stack_cutoff);
        printf("WORK_CHUNK_SIZE: %d\n", work_chunk_size);
    }

    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }

    srand(60);

    initialize_grid(matrix, n, n, random);
    if (rank == 0) {
        print_grid(matrix, n, n);
    }

    bool **visited = createVisitedMatrix(n, n);

    double total_time = 0.0;

    int iterations = benchmark_mode ? 10 : 1;

    for (int iter = 0; iter < iterations; iter++) {
        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();

        for (int i = 0; i < n; i++)
            MPI_Bcast(matrix[i], n, MPI_INT, 0, MPI_COMM_WORLD);

        generateConfigurations(matrix, n, n, visited, rank, size, stack_cutoff, work_chunk_size, compressed_state_type, benchmark_mode);

        MPI_Barrier(MPI_COMM_WORLD);
        double end_time = MPI_Wtime();

        if (rank == 0) {
            double elapsed_time = end_time - start_time;
            total_time += elapsed_time;
            if (!benchmark_mode) {
                printf("Parallel execution time: %f seconds\n", elapsed_time);
            }
        }
    }

    if (rank == 0 && benchmark_mode) {
        double average_time = total_time / 10;
        printf("Average execution time over 10 iterations: %f seconds\n", average_time);

        // Costruisci il comando per eseguire lo script Python
        char command[512];
        snprintf(command, sizeof(command), "python3 ../benchmark.py %d %f %d results", size, average_time, n);

        // Esegui lo script Python
        execute_command(command);
    }

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);

    freeVisitedMatrix(visited, n);

    MPI_Type_free(&compressed_state_type);

    printf("Process %d: Finalizing\n", rank);
    MPI_Finalize();
    return 0;
}
