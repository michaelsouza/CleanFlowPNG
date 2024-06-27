#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 16

// CUDA kernel for updating the matrix
__global__ void updateMatrixKernel(int* d_matrix, int* d_new_matrix, int m, int n, bool* d_updated) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < m && col < n) {
        int current_value = d_matrix[row * n + col];
        if (current_value == 0 || current_value == 1) {
            int count[5] = {0};  // Count for each value (0-4)
            
            // Check 3x3 neighborhood with periodic boundary conditions
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;  // Skip the center cell
                    int neighbor_row = (row + i + m) % m;
                    int neighbor_col = (col + j + n) % n;
                    int neighbor_value = d_matrix[neighbor_row * n + neighbor_col];
                    count[neighbor_value]++;
                }
            }

            // Find the most frequent value (smallest in case of a tie)
            int max_count = 0;
            int new_value = current_value;
            for (int i = 0; i < 5; i++) {
                if (count[i] > max_count || (count[i] == max_count && i < new_value)) {
                    max_count = count[i];
                    new_value = i;
                }
            }

            // Update the cell if necessary
            if (new_value != current_value) {
                d_new_matrix[row * n + col] = new_value;
                *d_updated = true;
            } else {
                d_new_matrix[row * n + col] = current_value;
            }
        } else {
            d_new_matrix[row * n + col] = current_value;
        }
    }
}

// Host function to update the matrix
void updateMatrix(int* matrix, int m, int n) {
    int* d_matrix;
    int* d_new_matrix;
    bool* d_updated;
    bool updated;

    size_t size = m * n * sizeof(int);

    // Allocate device memory
    cudaMalloc((void**)&d_matrix, size);
    cudaMalloc((void**)&d_new_matrix, size);
    cudaMalloc((void**)&d_updated, sizeof(bool));

    // Copy input matrix to device
    cudaMemcpy(d_matrix, matrix, size, cudaMemcpyHostToDevice);

    // Set up grid and block dimensions
    dim3 blockDim(BLOCK_SIZE, BLOCK_SIZE);
    dim3 gridDim((n + BLOCK_SIZE - 1) / BLOCK_SIZE, (m + BLOCK_SIZE - 1) / BLOCK_SIZE);

    do {
        // Reset the updated flag
        updated = false;
        cudaMemcpy(d_updated, &updated, sizeof(bool), cudaMemcpyHostToDevice);

        // Launch the kernel
        updateMatrixKernel<<<gridDim, blockDim>>>(d_matrix, d_new_matrix, m, n, d_updated);

        // Swap the matrices
        int* temp = d_matrix;
        d_matrix = d_new_matrix;
        d_new_matrix = temp;

        // Check if any updates were made
        cudaMemcpy(&updated, d_updated, sizeof(bool), cudaMemcpyDeviceToHost);
    } while (updated);

    // Copy the result back to host
    cudaMemcpy(matrix, d_matrix, size, cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_matrix);
    cudaFree(d_new_matrix);
    cudaFree(d_updated);
}

// Main function for testing
int main() {
    int m = 5, n = 5;
    int matrix[25] = {
        0, 1, 2, 3, 4,
        1, 2, 3, 4, 0,
        2, 3, 4, 0, 1,
        3, 4, 0, 1, 2,
        4, 0, 1, 2, 3
    };

    printf("Original matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i * n + j]);
        }
        printf("\n");
    }

    updateMatrix(matrix, m, n);

    printf("\nUpdated matrix:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", matrix[i * n + j]);
        }
        printf("\n");
    }

    return 0;
}