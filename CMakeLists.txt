cmake_minimum_required(VERSION 3.23)
project(parallel_hitori C)

set(CMAKE_C_STANDARD 11)

# Trova il pacchetto MPI
find_package(MPI REQUIRED)

# Include la directory corrente
include_directories(.)

# Imposta le opzioni di compilazione
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Ofast -march=native -funroll-all-loops")
set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -lm")

# Aggiungi il file sorgente e crea l'eseguibile
add_executable(parallel_hitori
        hitoriseqfunctions.h
        phitori.c)

# Link MPI librerie
target_link_libraries(parallel_hitori ${MPI_LIBRARIES})

# Include MPI directory
include_directories(${MPI_INCLUDE_PATH})
