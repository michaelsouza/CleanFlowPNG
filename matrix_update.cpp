#include <vector>
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <random>

/**
 * @brief Update matrix cells based on neighborhood frequencies.
 * 
 * This function updates a matrix where cells with values 0 or 1 are replaced by
 * the most frequent value in their 3x3 neighborhood. The process is repeated
 * until no further updates are needed. Periodic boundary conditions are applied.
 * 
 * @param matrix The input matrix to be updated (modified in-place).
 * @return The number of iterations performed.
 */
int updateMatrix(std::vector<std::vector<int>>& matrix) {
    int m = matrix.size();
    int n = matrix[0].size();
    int iterations = 0;
    bool updated;

    // Create a copy of the matrix for reading while updating
    std::vector<std::vector<int>> temp_matrix(m, std::vector<int>(n));

    do {
        updated = false;
        temp_matrix = matrix;

        #pragma omp parallel for collapse(2) shared(matrix, temp_matrix, updated)
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (matrix[i][j] == 0 || matrix[i][j] == 1) {
                    std::vector<int> freq(5, 0);  // Frequency count for values 0-4

                    // Count frequencies in 3x3 neighborhood
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (di == 0 && dj == 0) continue;  // Skip the cell itself
                            int ni = (i + di + m) % m;  // Apply periodic boundary conditions
                            int nj = (j + dj + n) % n;
                            ++freq[temp_matrix[ni][nj]];
                        }
                    }

                    // Find the most frequent value (smallest if tied)
                    int max_freq = 0;
                    int new_value = matrix[i][j];
                    for (int k = 0; k < 5; ++k) {
                        if (freq[k] > max_freq || (freq[k] == max_freq && k < new_value)) {
                            max_freq = freq[k];
                            new_value = k;
                        }
                    }

                    // Update the cell if necessary
                    if (new_value != matrix[i][j]) {
                        matrix[i][j] = new_value;
                        updated = true;
                    }
                }
            }
        }

        ++iterations;
    } while (updated);

    return iterations;
}

/**
 * @brief Print a matrix to the console.
 * 
 * @param matrix The matrix to be printed.
 */
void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Set matrix dimensions
    const int m = 10;
    const int n = 10;

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 4);

    // Create and initialize the matrix with random values
    std::vector<std::vector<int>> matrix(m, std::vector<int>(n));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = dis(gen);
        }
    }

    // Print initial matrix
    std::cout << "Initial matrix:" << std::endl;
    printMatrix(matrix);

    // Update the matrix
    int iterations = updateMatrix(matrix);

    // Print results
    std::cout << "\nFinal matrix after " << iterations << " iterations:" << std::endl;
    printMatrix(matrix);

    return 0;
}