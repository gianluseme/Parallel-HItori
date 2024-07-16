#!/bin/bash

# Controlla il numero di argomenti
if [ "$#" -lt 4 ]; then
  echo "Usage: $0 <num_processes> <grid_size> <stack_cutoff> <work_chunk_size1> ... <work_chunk_sizeN>"
  exit 1
fi

# Leggi gli argomenti della linea di comando
num_processes=$1
grid_size=$2
stack_cutoff=$3
shift 3  # Sposta i primi tre argomenti
work_chunk_sizes=("$@")

# Verifica che il numero di valori di work chunk size sia uguale al numero di processi
if [ "${#work_chunk_sizes[@]}" -ne "$num_processes" ]; then
  echo "Error: The number of work chunk sizes must be equal to the number of processes."
  exit 1
fi

# Trova e spostati nella directory corretta
if [ -d "cmake-build-debug" ]; then
  cd cmake-build-debug
elif [ -d "build" ]; then
  cd build
else
  echo "Error: Neither cmake-build-debug nor build directory exists."
  exit 1
fi

# Compila il programma se necessario (assumendo un Makefile è presente)
# make

# Esegui il programma per ogni numero di processi e work chunk size
for (( i=1; i<=$num_processes; i++ )); do
  work_chunk_size=${work_chunk_sizes[$((i-1))]}
  echo "Running benchmark with $i processes, grid size $grid_size, stack cutoff $stack_cutoff, work chunk size $work_chunk_size"
  mpirun -np $i ./parallel_hitori -n $grid_size -c $stack_cutoff -w $work_chunk_size -b
done

# Torna alla directory precedente per eseguire lo script Python
cd ..

# Esegui lo script Python per il plotting
python3 plot_results.py $grid_size
