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

// Type.
#include <Type.hpp>

// Matrix.
#include <Matrix.hpp>

// Containers.
#include <vector>

// Chrono.
#include <chrono>

namespace pacs {

    namespace algebra {

        /**
         * @brief Prints the time needed for an arbitrary number of Matrix x Vector products, along with some other info.
         * 
         * @tparam T 
         * @tparam O 
         * @param matrix 
         * @param vector 
         */
        template<MatrixType T, Order O>
        void timer(const Matrix<T, O> &matrix, const std::vector<T> &vector, const std::size_t &tests = 5E5) {
            std::cout << "\nTesting for Matrix x Vector product." << std::endl;
            std::size_t products = 0;

            // Tests.
            auto start = std::chrono::high_resolution_clock::now();

            // Matrix x Vector.
            if(matrix.columns() == vector.size()) {
                std::cout << "- Testing Matrix x Vector, elapsed: ";
                ++products;

                for(std::size_t j = 0; j < tests; ++j)
                    matrix * vector;

                std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1E6 << " second(s)." << std::endl;
            }

            // Vector x Matrix.
            if(matrix.rows() == vector.size()) {
                std::cout << "- Testing Vector x Matrix, elapsed: ";
                ++products;
                
                for(std::size_t j = 0; j < tests; ++j)
                    vector * matrix;

                std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1E6 << " second(s)." << std::endl;
            }

            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\nTested " << products * tests << " (" << products << " x " << tests << ") products between a " << matrix.rows() << " by " << matrix.columns() << " sparse matrix and a " << vector.size() << " (by 1) vector." << std::endl;
            std::cout << "Matrix has a sparsity of " << matrix.sparsity() << ", it is " << (O == Row ? "row-first" : "column-first") << " ordered and it is " << (matrix.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;
        }

        /**
         * @brief Prints the time needed for an arbitrary number of Matrix x Matrix products, along with some other info.
         * 
         * @tparam T 
         * @tparam O 
         * @param matrix 
         * @param vector 
         */
        template<MatrixType T, Order O>
        void timer(const Matrix<T, O> &first, const Matrix<T, O> &second, const std::size_t &tests = 1E3) {
            std::cout << "\nTesting for Matrix x Vector product." << std::endl;

            // Tests.
            auto start = std::chrono::high_resolution_clock::now();

            std::cout << "- Testing Matrix x Matrix." << std::endl;

            for(std::size_t j = 0; j < tests; ++j)
                first * second;

            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\nTested " << tests << " products between a " << first.rows() << " by " << first.columns() << " sparse matrix and a " << second.rows() << " by " << second.columns() << " sparse matrix." << std::endl;
            std::cout << "First one has a sparsity of " << first.sparsity() << " and it is " << (first.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Second one has a sparsity of " << second.sparsity() << " and it is " << (second.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Both matrices are " << (O == Row ? "row-first" : "column-first") << " ordered." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;
        }

    }

}

#endif