/**
 * @file main.cpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>

// Includes.
#include "main.hpp"
using namespace pacs; // For ease of reading.

int main(int argc, char **argv) {
    // Default "splash".
    std::cout << "PACS - Second challenge - Andrea Di Antonio." << std::endl;

    #ifdef PARALLEL_PACS
        std::cout << "Enabled parallel computing." << std::endl;
    #endif

    // Test "subjects".
    algebra::Matrix<double> row_matrix = algebra::market<double>("data/matrix.mtx");
    algebra::Matrix<double, algebra::Column> column_matrix = algebra::market<double, algebra::Column>("data/matrix.mtx");

    algebra::Matrix<double> row_matrix_1 = row_matrix;
    algebra::Matrix<double, algebra::Column> column_matrix_1 = column_matrix;

    std::vector<double> vector;
    vector.resize(row_matrix.rows(), 1.0);

    // Tests.

    // Vector product.

    // Uncompressed row-first matrix.
    algebra::timer(row_matrix, vector);

    // Compressed row-first matrix.
    // This should be the fastest.
    row_matrix.compress();
    algebra::timer(row_matrix, vector);

    // Uncompressed column-first matrix.
    algebra::timer(column_matrix, vector);

    // Compressed column-first matrix.
    column_matrix.compress();
    algebra::timer(column_matrix, vector);

    // Matrix product.
    
    row_matrix.uncompress();
    column_matrix.uncompress();

    // Uncompressed row-first x uncompressed row-first.
    algebra::timer(row_matrix, row_matrix_1);
    
    // Uncompressed row-first x compressed row-first.
    row_matrix_1.compress();
    algebra::timer(row_matrix, row_matrix_1);

    // compressed row-first x compressed row-first.
    row_matrix.compress();
    algebra::timer(row_matrix, row_matrix_1);

    // Uncompressed column-first x uncompressed column-first.
    algebra::timer(column_matrix, column_matrix_1);
    
    // Uncompressed column-first x compressed column-first.
    column_matrix_1.compress();
    algebra::timer(column_matrix, column_matrix_1);

    // compressed column-first x compressed column-first.
    column_matrix.compress();
    algebra::timer(column_matrix, column_matrix_1);
    

    return 0;
}