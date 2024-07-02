#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <iostream>
#include <png++/png.hpp>
#include <omp.h>

enum Color {
    GREEN = 0,
    RED = 1,
    WHITE = 2,
    ORANGE = 3,
    GRAY = 4,
    CYAN = 5,
};


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

/**
 * @brief Update matrix cells based on neighborhood frequencies.
 * 
 * This function updates a matrix where cells with values 0 or 1 are replaced by
 * the most frequent value in their 3x3 neighborhood. The process is repeated
 * until no further updates are needed. Periodic boundary conditions are applied.
 * 
 * @param matrix The input matrix to be updated (modified in-place). 
 * @param verbose If true, print the matrix at each iteration.
 * @return The number of iterations performed.
 */
int updateMatrix(std::vector<std::vector<int>>& matrix, bool verbose = false) {
    // Get matrix dimensions
    int m = matrix.size();
    int n = matrix[0].size();

    // Initialize iteration counter and update flag
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
                if (matrix[i][j] == WHITE || matrix[i][j] == ORANGE) {
                    std::vector<int> freq(4, 0);  // Frequency count for values 0-5

                    // Count frequencies in 3x3 neighborhood
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (di == 0 && dj == 0) continue;  // Skip the cell itself
                            int ni = (i + di + m) % m;  // Apply periodic boundary conditions
                            int nj = (j + dj + n) % n;
                            ++freq[temp_matrix[ni][nj]];
                        }
                    }

                    // Find the most frequent value
                    int max_freq = 0;
                    int new_value = matrix[i][j];
                    for (size_t k = 0; k < freq.size(); ++k) {
                        // in case of tie, keep the current value
                        if (freq[k] == max_freq) {
                            new_value = matrix[i][j];                            
                            continue;
                        }

                        if (freq[k] > max_freq) {
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

        if (verbose){
            printf("\n\nIteration %d\n", iterations);
            printMatrix(matrix);
        }

        ++iterations;
    } while (updated && iterations < 1000);

    return iterations;
}

void randomTest() {
    // Set matrix dimensions
    const int m = 10;
    const int n = 10;

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 5);    

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
}

std::vector<std::vector<int>> readPng(char *filename){
    printf("Reading image %s\n", filename);
    png::image<png::rgb_pixel> image(filename);
    std::vector<std::vector<int>> matrix(image.get_height(), std::vector<int>(image.get_width()));
    for (size_t i = 0; i < image.get_height(); ++i) {
        for (size_t j = 0; j < image.get_width(); ++j) {
            png::rgb_pixel pixel = image[i][j]; 
            if( pixel.red == 192 && pixel.green == 192 && pixel.blue == 192){
                matrix[i][j] = GRAY;
            } else if( pixel.red == 121 && pixel.green == 254 && pixel.blue == 89){
                matrix[i][j] = GREEN;
            } else if( pixel.red == 200 && pixel.green == 0 && pixel.blue == 0){
                matrix[i][j] = RED;
            } else if( pixel.red == 255 && pixel.green == 255 && pixel.blue == 255){
                matrix[i][j] = WHITE;
            } else if( pixel.red == 222 && pixel.green == 62 && pixel.blue == 7){
                matrix[i][j] = ORANGE;
            } else if( pixel.red == 0 && pixel.green == 255 && pixel.blue == 255){
                matrix[i][j] = CYAN;
            } else {
                printf("Unknown color at %ld %ld\n", i, j);
            }
        }
    }
    return matrix;
}

void saveMatrix(std::vector<std::vector<int>> matrix, char *filename){
    png::image<png::rgb_pixel> image(matrix[0].size(), matrix.size());
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[0].size(); ++j) {
            png::rgb_pixel pixel;
            switch (matrix[i][j]){
                case GREEN:
                    pixel.red = 121;
                    pixel.green = 254;
                    pixel.blue = 89;
                    break;
                case RED:
                    pixel.red = 200;
                    pixel.green = 0;
                    pixel.blue = 0;
                    break;
                case WHITE:
                    pixel.red = 255;
                    pixel.green = 255;
                    pixel.blue = 255;
                    break;
                case ORANGE:
                    pixel.red = 222;
                    pixel.green = 62;
                    pixel.blue = 7;
                    break;
                case GRAY:
                    pixel.red = 192;
                    pixel.green = 192;
                    pixel.blue = 192;
                    break;
                case CYAN:
                    pixel.red = 0;
                    pixel.green = 255;
                    pixel.blue = 255;
                    break;
                default:
                    pixel.red = 0;
                    pixel.green = 0;
                    pixel.blue = 0;
                    break;
            }
            image[i][j] = pixel;
        }
    }
    image.write(filename);
}

int main(int argc, char** argv) {
    // randomTest();

    std::string filename = "/home/michael/gitrepos/PixBalance/images/phase2_000000000.000000.png";
    if(argc > 1){
        filename = argv[1];        
    }

    auto matrix = readPng((char*)filename.c_str());

    auto iterations = updateMatrix(matrix);

    printf("Iterations: %d\n", iterations);
    
    filename = filename.substr(0, filename.size()-4) + "_output.png";
    saveMatrix(matrix, (char*)filename.c_str());

    return EXIT_SUCCESS;
}
