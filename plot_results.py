import sys
import os
import csv
import matplotlib.pyplot as plt

def read_results(results_file):
    processes = []
    execution_times = []
    ideal_execution_times = []
    speedups = []
    efficiencies = []

    try:
        with open(results_file, 'r') as file:
            reader = csv.DictReader(file)
            for row in reader:
                processes.append(int(row['p']))
                execution_times.append(float(row['tempo di esecuzione']))
                ideal_execution_times.append(float(row['tempo di esecuzione ideale']))
                speedups.append(float(row['speedup']))
                efficiencies.append(float(row['efficienza']))
    except FileNotFoundError:
        print(f"File not found: {results_file}")
        sys.exit(1)

    return processes, execution_times, ideal_execution_times, speedups, efficiencies

def plot_execution_time(processes, execution_times, ideal_execution_times, matrix_size, results_dir):
    plt.figure()
    plt.plot(processes, execution_times, marker='o', label='Execution Time')
    plt.plot(processes, ideal_execution_times, marker='o', linestyle='--', label='Ideal Execution Time')
    plt.title('Execution Time vs Number of Processes')
    plt.xlabel('Number of Processes')
    plt.ylabel('Execution Time (seconds)')
    plt.xticks(processes)  # Set exact p values on x-axis
    plt.grid(True)
    plt.legend()
    plt.savefig(os.path.join(results_dir, f'results_execution_time_{matrix_size}.png'))
    plt.show()

def plot_speedup(processes, speedups, matrix_size, results_dir):
    plt.figure()
    plt.plot(processes, speedups, marker='o')
    plt.title('Speedup vs Number of Processes')
    plt.xlabel('Number of Processes')
    plt.ylabel('Speedup')
    plt.xticks(processes)  # Set exact p values on x-axis
    plt.grid(True)
    plt.savefig(os.path.join(results_dir, f'results_speedup_{matrix_size}.png'))
    plt.show()

def plot_efficiency(processes, efficiencies, matrix_size, results_dir):
    plt.figure()
    plt.plot(processes, efficiencies, marker='o')
    plt.title('Efficiency vs Number of Processes')
    plt.xlabel('Number of Processes')
    plt.ylabel('Efficiency')
    plt.xticks(processes)  # Set exact p values on x-axis
    plt.grid(True)
    plt.savefig(os.path.join(results_dir, f'results_efficiency_{matrix_size}.png'))
    plt.show()

def main():
    if len(sys.argv) != 2:
        print("Usage: plot_results.py <matrix_size>")
        sys.exit(1)

    matrix_size = int(sys.argv[1])
    results_dir = 'results'
    results_file = os.path.join(results_dir, f'results{matrix_size}.csv')

    processes, execution_times, ideal_execution_times, speedups, efficiencies = read_results(results_file)

    plot_execution_time(processes, execution_times, ideal_execution_times, matrix_size, results_dir)
    plot_speedup(processes, speedups, matrix_size, results_dir)
    plot_efficiency(processes, efficiencies, matrix_size, results_dir)

if __name__ == "__main__":
    main()
