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
#include <chrono>

// Includes.
#include "main.hpp"
using namespace pacs; // For ease of reading.

int main(int argc, char **argv) {
    // Default "splash".
    std::cout << "PACS - Second challenge - Andrea Di Antonio." << std::endl;

    // Test "subjects".
    algebra::Matrix<double> row_matrix = algebra::market<double>("data/matrix.mtx");
    algebra::Matrix<double, algebra::Column> column_matrix = algebra::market<double, algebra::Column>("data/matrix.mtx");

    std::vector<double> vector;
    vector.resize(row_matrix.rows(), 1.0);

    // Tests.

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

    return 0;
}