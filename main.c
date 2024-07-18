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

    double seq_time_create_type_start, seq_time_create_type_end, total_seq_time_create_type = 0.0;
    double seq_time_initialize_grid_start, seq_time_initialize_grid_end, total_seq_time_initialize_grid = 0.0;
    double seq_time_print_start, seq_time_print_end, total_seq_time_print = 0.0;

    seq_time_create_type_start = MPI_Wtime();
    MPI_Datatype compressed_state_type = create_compressed_state_type();
    seq_time_create_type_end = MPI_Wtime();
    total_seq_time_create_type += (seq_time_create_type_end - seq_time_create_type_start);

    printf("Process %d: Starting, total processes: %d\n", rank, size);

    int n = 0;
    bool random = true;
    int stack_cutoff = 0;
    int work_chunk_size = 0;
    bool benchmark_mode = false;
    bool brute_force = false;

    double seq_time_start, seq_time_end, total_seq_time;

    int option;
    while ((option = getopt(argc, argv, "n:pc:w:bf")) != -1) {
        switch (option) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'p':
                random = false;
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
            case 'f':
                brute_force = true;
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

    if(!random)  {
        if(brute_force)n = 5;
        else n = 8;
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

    // Inizializzazione della griglia (sequenziale)
    seq_time_initialize_grid_start = MPI_Wtime();
    int **matrix = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        matrix[i] = malloc(n * sizeof(int));
    }

    srand(60);

    seq_time_initialize_grid_end = MPI_Wtime();
    total_seq_time_initialize_grid += (seq_time_initialize_grid_end - seq_time_initialize_grid_start);

    if (rank == 0) {
        initialize_grid(matrix, n, random, brute_force);
        seq_time_print_start = MPI_Wtime();
        print_grid(matrix, n);
        seq_time_print_end = MPI_Wtime();
        total_seq_time_print += (seq_time_print_end - seq_time_print_start);
    }

    bool **visited = createVisitedMatrix(n);

    double seq_work_time = 0.0;

    int iterations = benchmark_mode ? 10 : 1;

    if(brute_force)
        iterations = 1;

    double total_seq_time_accumulated = 0.0;
    double total_time_accumulated = 0.0;
    double local_total_seq_time;

    // Array per memorizzare i tempi sequenziali per ogni iterazione
    double *seq_times = malloc(iterations * sizeof(double));
    double *seq_work_times = malloc(iterations * sizeof(double));  // Array per seq_work_time


    for (int iter = 0; iter < iterations; iter++) {

        total_seq_time_encode_stack = 0;
        total_seq_time_request_work = 0;
        total_seq_time_handle_requests = 0;
        total_seq_time_split_work = 0;

        MPI_Barrier(MPI_COMM_WORLD);
        double start_time = MPI_Wtime();

        seq_time_start = MPI_Wtime();
        for (int i = 0; i < n; i++)
            MPI_Bcast(matrix[i], n, MPI_INT, 0, MPI_COMM_WORLD);
        seq_time_end = MPI_Wtime();
        total_seq_time = (seq_time_end - seq_time_start);

        if(!brute_force) {
            seq_work_time = generateConfigurations(matrix, n, visited, rank, size, stack_cutoff, work_chunk_size,
                                                   compressed_state_type, benchmark_mode);
        } else
            generate_solution(matrix, n, rank, size, visited);


        local_total_seq_time = total_seq_time_create_type + total_seq_time_initialize_grid + total_seq_time_print + total_seq_time_handle_requests + total_seq_time_split_work + total_seq_time_request_work + total_seq_time_encode_stack + total_seq_time;

        seq_times[iter] = local_total_seq_time;
        seq_work_times[iter] = seq_work_time;
        MPI_Barrier(MPI_COMM_WORLD);
        double end_time = MPI_Wtime();

        double elapsed_time = end_time - start_time;
        total_time_accumulated += elapsed_time;

        if (!benchmark_mode && rank == 0) {
            printf("Parallel execution time: %f seconds\n", elapsed_time);
        }
    }

    // Calcolo della media dei tempi sequenziali e seq_work_time
    for (int iter = 0; iter < iterations; iter++) {
        total_seq_time_accumulated += seq_times[iter];
        seq_work_time += seq_work_times[iter];
    }
    total_seq_time_accumulated /= iterations;
    seq_work_time /= iterations;

    double avg_seq_work_time = seq_work_time / iterations;
    double avg_total_time = total_time_accumulated / iterations;

    // Variabile per raccogliere il tempo sequenziale totale e il tempo totale
    double global_total_seq_time;
    global_total_seq_time = total_seq_time_accumulated + avg_seq_work_time;

    if (rank == 0 && benchmark_mode) {

        // Costruisci il comando per eseguire lo script Python
        char command[512];
        snprintf(command, sizeof(command), "python3 ../benchmark.py %d %f %d results %f", size, avg_total_time, n, global_total_seq_time);

        // Esegui lo script Python
        execute_command(command);
    }

    if (rank == 0 && size > 1) {
        printf("Total sequential time: %f seconds\n", global_total_seq_time);
        printf("Total execution time: %f seconds\n", avg_total_time);
    }

    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);

    freeVisitedMatrix(visited, n);

    MPI_Type_free(&compressed_state_type);

    free(seq_times);
    free(seq_work_times);


    printf("Process %d: Finalizing\n", rank);
    MPI_Finalize();

    return 0;
}

