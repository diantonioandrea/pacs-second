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

#include <complex>

int main(int argc, char **argv) {
    // Default "splash".
    std::cout << "PACS - Second challenge - Andrea Di Antonio." << std::endl;

    pacs::algebra::Matrix<double, pacs::algebra::Row> matrix{2, 2};

    matrix(0, 0) = 4;
    matrix(1, 1) = 3;

    matrix.compress();
    std::cout << matrix.norm<pacs::algebra::Frobenius>() << std::endl;

    return 0;
}