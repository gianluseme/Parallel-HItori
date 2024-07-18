//
// Created by semeg on 26/06/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>


void print_grid(int **matrix, int n) {
    printf("Grid:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_solution(char **status, int n, int **matrix) {
    printf("Solution:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if(status[i][j] == 'X')
                printf("█ ");
            else printf("%d ",matrix[i][j]);
        }
        printf("\n");
    }
}

void shuffle(int *array, int size) {
    if (size > 1) {
        for (int i = 0; i < size - 1; i++) {
            int j = i + rand() / (RAND_MAX / (size - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void initialize_grid(int **matrix, int n, bool random) {
    if (random) {
        int *numbers = malloc(n * n * sizeof(int));
        int max_duplicates = 1;
        for (int i = 0; i < n * n; i++) {
            numbers[i] = i % (n / max_duplicates) + 1;
        }
        shuffle(numbers, n * n);
        int count = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                matrix[i][j] = numbers[count++];
            }
        }
        free(numbers);
    } else {
        // Griglia predefinita (modificare secondo necessità per il caso rettangolare)
        int values[8][8] = {
                {4, 8, 7, 2, 3, 2, 1, 2},
                {5, 6, 1, 3, 4, 1, 5, 7},
                {7, 2, 2, 8, 2, 1, 7, 3},
                {5, 7, 1, 4, 8, 3, 6, 4},
                {7, 1, 5, 2, 2, 7, 5, 4},
                {6, 4, 1, 7, 5, 2, 8, 1},
                {3, 7, 6, 5, 6, 4, 2, 8},
                {3, 3, 4, 6, 2, 8, 7, 1}
        };
        for (int i = 0; i < n && i < 8; i++) {
            for (int j = 0; j < n && j < 8; j++) {
                matrix[i][j] = values[i][j];
            }
        }
    }
}


bool** createVisitedMatrix(int n) {
    bool** matrix = (bool**)malloc(n * sizeof(bool*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (bool*)malloc(n * sizeof(bool));
    }
    return matrix;
}

void freeVisitedMatrix(bool** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Funzione DFS ottimizzata
void dfs(int row, int col, bool **visited, char **board, int n) {
    if (row < 0 || row >= n || col < 0 || col >= n || visited[row][col] || board[row][col] == 'X') {
        return;
    }
    visited[row][col] = true;
    dfs(row - 1, col, visited, board, n);
    dfs(row + 1, col, visited, board, n);
    dfs(row, col - 1, visited, board, n);
    dfs(row, col + 1, visited, board, n);
}

// Funzione per verificare la presenza di un'isola
bool hasIsland(char **board, bool **visited, int n) {
    // Trova un punto non visitato
    int startRow = -1, startCol = -1;
    for (int i = 0; i < n && startRow == -1; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] != 'X') {
                startRow = i;
                startCol = j;
                break;
            }
        }
    }

    // Se non ci sono celle non bloccate, non ci sono isole
    if (startRow == -1) return false;

    // Inizializza la matrice visited
    for (int i = 0; i < n; i++) {
        memset(visited[i], false, n * sizeof(bool));
    }

    // Esegui DFS a partire dal primo punto trovato
    dfs(startRow, startCol, visited, board, n);

    // Verifica se c'è qualche cella non visitata che non è 'X'
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] != 'X' && !visited[i][j]) {
                return true; // C'è un'isola
            }
        }
    }
    return false; // Tutto connesso
}


// Funzione per verificare se una configurazione di griglia è valida
bool is_valid(int **matrix, char ** status, int n, int m) {
    // Verifica righe e colonne
    for (int i = 0; i < n; i++) {
        bool used[m + 1];
        memset(used, false, sizeof(used));

        for (int j = 0; j < m; j++) {
            // Verifica righe per duplicati
            if (status[i][j]!= 'X') {
                int value = matrix[i][j];
                if (value != 0 && used[value]) {
                    return false;
                }
                used[value] = true;
            }
        }
    }

    // Verifica colonne per duplicati
    for (int j = 0; j < m; j++) {
        bool used[n + 1];
        memset(used, false, sizeof(used));

        for (int i = 0; i < n; i++) {
            if (status[i][j] != 'X') {
                int value = matrix[i][j];
                if (value != 0 && used[value]) {
                    return false;
                }
                used[value] = true;
            }
        }
    }

    return true;
}

// Funzione per verificare se è sicuro mettere una X in una posizione (row, col)
int isSafe(char **status, int rows, int cols, int row, int col, bool **visited, int ** grid) {
    // Controllo celle adiacenti per X
    int dR[] = {-1, 1, 0, 0};
    int dC[] = {0, 0, -1, 1};
    for (int k = 0; k < 4; k++) {
        int newRow = row + dR[k];
        int newCol = col + dC[k];
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols && status[newRow][newCol] == 'X') {
            return 0;
        }
    }

    // Verifica che ci sia almeno un duplicato nella riga o colonna con stato '.'
    bool hasDuplicateRow = false;
    bool hasDuplicateCol = false;

    // Controllo duplicati nella riga
    for (int i = 0; i < cols; i++) {
        if (i != col && grid[row][i] == grid[row][col] && status[row][i] == '.') {
            hasDuplicateRow = true;
            break;
        }
    }

    // Controllo duplicati nella colonna
    for (int i = 0; i < rows; i++) {
        if (i != row && grid[i][col] == grid[row][col] && status[i][col] == '.') {
            hasDuplicateCol = true;
            break;
        }
    }

    if (!hasDuplicateRow && !hasDuplicateCol) {
        return 0;
    }

    // Temporaneamente piazza una 'X'
    status[row][col] = 'X';

    // Verifica che non ci siano blocchi 2x2 di 'X'
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            if (status[i][j] == 'X' && status[i + 1][j] == 'X' &&
                status[i][j + 1] == 'X' && status[i + 1][j + 1] == 'X') {
                status[row][col] = '.'; // Ripristina lo stato originale
                return 0;
            }
        }
    }

    // Evita di isolare una porzione della griglia
    bool isolated = false;
    for (int k = 0; k < 4; k++) {
        int newRow = row + dR[k];
        int newCol = col + dC[k];
        if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols && status[newRow][newCol] == '.') {
            bool allAdjacentX = true;
            for (int l = 0; l < 4; l++) {
                int adjRow = newRow + dR[l];
                int adjCol = newCol + dC[l];
                if (adjRow >= 0 && adjRow < rows && adjCol >= 0 && adjCol < cols && status[adjRow][adjCol] != 'X') {
                    allAdjacentX = false;
                    break;
                }
            }
            if (allAdjacentX) {
                isolated = true;
                break;
            }
        }
    }

    if (isolated) {
        status[row][col] = '.'; // Ripristina lo stato originale
        return 0;
    }

    // Verifica che la riga non abbia già il numero massimo di 'X'
    int countXRow = 0;
    for (int i = 0; i < cols; i++) {
        if (status[row][i] == 'X') {
            countXRow++;
        }
    }
    if (countXRow >= cols - 1) {
        status[row][col] = '.'; // Ripristina lo stato originale
        return 0;
    }

    // Verifica che la colonna non abbia già il numero massimo di 'X'
    int countXCol = 0;
    for (int i = 0; i < rows; i++) {
        if (status[i][col] == 'X') {
            countXCol++;
        }
    }
    if (countXCol >= rows - 1) {
        status[row][col] = '.'; // Ripristina lo stato originale
        return 0;
    }

    // Verifica che non ci siano blocchi diagonali di 'X'
    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            if (status[i][j] == 'X' && status[i + 1][j + 1] == 'X' &&
                status[i + 1][j] == 'X' && status[i][j + 1] == 'X') {
                status[row][col] = '.'; // Ripristina lo stato originale
                return 0;
            }
        }
    }



    // Verifica la connettività
    bool isConnected = !hasIsland(status, visited, rows);

    status[row][col] = '.'; // Ripristina lo stato originale

    return isConnected;
}

// Funzione per convertire la griglia in un numero intero univoco
uint64_t gridToNumber(char **grid, int rows) {
    uint64_t number = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            if (grid[i][j] == 'X') {
                number |= ((uint64_t)1 << (i * rows + j));
            }
        }
    }
    return number;
}

// Funzione per convertire un numero intero in una griglia
void numberToGrid(uint64_t number, char **grid, int rows) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows; j++) {
            grid[i][j] = (number & ((uint64_t)1 << (i * rows + j))) ? 'X' : '.';
        }
    }
}

void execute_command(const char *command) {
    int result = system(command);
    if (result == -1) {
        perror("system");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
}
