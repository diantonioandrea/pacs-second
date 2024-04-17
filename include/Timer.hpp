/**
 * @file Timer.hpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TIMER_PACS
#define TIMER_PACS

// Containers.
#include <vector>

// Chrono.
#include <chrono>

// Matrix.
#include <Matrix.hpp>

namespace pacs {

    namespace algebra {

        /**
         * @brief Prints the time needed for an arbitrary number of Matrix x Vector products.
         * 
         * @tparam T 
         * @tparam O 
         * @param matrix 
         * @param vector 
         */
        template<typename T, Order O>
        void timer(const Matrix<T, O> &matrix, const std::vector<T> &vector, const std::size_t &tests = 5E5) {

            std::cout << "\nTesting for Matrix x Vector product." << std::endl;
            std::size_t products = 0;

            // Tests.
            auto start = std::chrono::high_resolution_clock::now();

            // Matrix x Vector.
            if(matrix.columns() == vector.size()) {
                std::cout << "- Testing Matrix x Vector." << std::endl;
                ++products;

                for(std::size_t j = 0; j < tests; ++j)
                    matrix * vector;
            }

            // Vector x Matrix.
            if(matrix.rows() == vector.size()) {
                std::cout << "- Testing Vector x Matrix." << std::endl;
                ++products;
                
                for(std::size_t j = 0; j < tests; ++j)
                    vector * matrix;
            }

            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\nTested " << products * tests << " (" << products << " x " << tests << ") products between a " << matrix.rows() << " by " << matrix.columns() << " sparse matrix and a " << vector.size() << " (by 1) vector." << std::endl;
            std::cout << "Matrix has a sparsity of " << matrix.sparsity() << ", it is " << (O == Row ? "row-first" : "column-first") << " ordered and it is " << (matrix.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;

        }

    }

}

#endif