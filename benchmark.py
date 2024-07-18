import sys
import os
import csv

def read_previous_execution_time(results_file):
    try:
        with open(results_file, 'r') as file:
            reader = csv.DictReader(file)
            for row in reader:
                if int(row['p']) == 1:
                    return float(row['tempo di esecuzione'])
    except FileNotFoundError:
        return None
    return None

def update_results_file(results_file, new_data):
    rows = []
    file_exists = os.path.isfile(results_file)

    if file_exists:
        with open(results_file, 'r') as file:
            reader = csv.DictReader(file)
            rows = list(reader)

    updated = False
    for i, row in enumerate(rows):
        if int(row['p']) == new_data['p']:
            rows[i] = new_data  # Aggiorna i dati esistenti
            updated = True
            break

    if not updated:
        rows.append(new_data)  # Aggiungi i nuovi dati

    # Ordina i dati per il numero di processi (p)
    rows.sort(key=lambda x: int(x['p']))

    with open(results_file, 'w', newline='') as csvfile:
        fieldnames = ['p', 'tempo di esecuzione', 'tempo di esecuzione ideale', 'speedup', 'efficienza', 'tempo sequenziale', 'amdahl', 'gustafson']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)

def main():
    if len(sys.argv) != 6:
        print("Usage: benchmark.py <p> <execution_time> <matrix_size> <results_dir> <sequential_fraction>")
        sys.exit(1)

    p = int(sys.argv[1])
    execution_time = float(sys.argv[2])
    matrix_size = int(sys.argv[3])
    results_dir = os.path.join("..", sys.argv[4])  # Cambia results_dir per puntare alla directory precedente
    sequential_fraction = float(sys.argv[5])

    results_file = os.path.join(results_dir, f'results{matrix_size}.csv')

    if not os.path.exists(results_dir):
        os.makedirs(results_dir)

    seq_time = execution_time
    ideal_execution_time = execution_time
    if p > 1:
        seq_time = read_previous_execution_time(results_file)
        if seq_time is None:
            print("Sequential execution time not found for p=1.")
            sys.exit(1)
        ideal_execution_time = seq_time / p

    speedup = seq_time / execution_time
    efficiency = speedup / p

    # Calcolare le metriche teoriche di Amdahl e Gustafson
    amdahl = 1 / (sequential_fraction + (1 - sequential_fraction) / p)
    gustafson = p - (p - 1) * sequential_fraction

    new_data = {
        'p': p,
        'tempo di esecuzione': execution_time,
        'tempo di esecuzione ideale': ideal_execution_time,
        'speedup': speedup,
        'efficienza': efficiency,
        'tempo sequenziale': sequential_fraction,
        'amdahl': amdahl,
        'gustafson': gustafson
    }

    update_results_file(results_file, new_data)

if __name__ == "__main__":
    main()
