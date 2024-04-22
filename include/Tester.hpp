/**
 * @file Tester.hpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef TESTER_PACS
#define TESTER_PACS

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
         * @param tests 
         */
        template<MatrixType T, Order O>
        void tester(const Matrix<T, O> &matrix, const std::vector<T> &vector, const std::size_t &tests = 5E5) {
            std::cout << "\nTesting for Matrix x Vector product." << std::endl;
            std::size_t products = 0;

            // Tests.
            auto start = std::chrono::high_resolution_clock::now();
            decltype(start) partial;

            // Matrix x Vector.
            if(matrix.columns() == vector.size()) {
                std::cout << "- Testing Matrix x Vector, elapsed: ";
                partial = std::chrono::high_resolution_clock::now();
                ++products;

                for(std::size_t j = 0; j < tests; ++j)
                    matrix * vector;

                std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - partial).count() / 1E6 << " second(s)." << std::endl;
            }

            // Vector x Matrix.
            if(matrix.rows() == vector.size()) {
                std::cout << "- Testing Vector x Matrix, elapsed: ";
                partial = std::chrono::high_resolution_clock::now();
                ++products;
                
                for(std::size_t j = 0; j < tests; ++j)
                    vector * matrix;

                std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - partial).count() / 1E6 << " second(s)." << std::endl;
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
         * @param first 
         * @param second 
         * @param tests 
         */
        template<MatrixType T, Order O>
        void tester(const Matrix<T, O> &first, const Matrix<T, O> &second, const std::size_t &tests = 1E3) {
            std::cout << "\nTesting for Matrix x Matrix product." << std::endl;

            // Test.
            auto start = std::chrono::high_resolution_clock::now();

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

        /**
         * @brief Prints the time needed for an arbitrary number of Matrix x Scalar products, along with some other info.
         * 
         * @tparam T 
         * @tparam O 
         * @param matrix 
         * @param scalar 
         * @param tests 
         */
        template<MatrixType T, Order O>
        void tester(const Matrix<T, O> &matrix, const T &scalar, const std::size_t &tests = 1E5) {
            std::cout << "\nTesting for Matrix x Scalar product." << std::endl;

            // Test.
            auto start = std::chrono::high_resolution_clock::now();

            for(std::size_t j = 0; j < tests; ++j)
                matrix * scalar;

            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\nTested " << tests << " products between a " << matrix.rows() << " by " << matrix.columns() << " sparse matrix and a scalar." << std::endl;
            std::cout << "Matrix has a sparsity of " << matrix.sparsity() << ", it is " << (O == Row ? "row-first" : "column-first") << " ordered and it is " << (matrix.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;
        }

        /**
         * @brief Prints the time needed for an arbitrary number of Matrix norm calculations, along with some other info.
         * 
         * @tparam T 
         * @tparam O 
         * @param matrix 
         * @param tests 
         */
        template<MatrixType T, Order O>
        void tester(const Matrix<T, O> &matrix, const std::size_t &tests = 5E5) {
            std::cout << "\nTesting for Matrix norm" << std::endl;

            // Test.
            auto start = std::chrono::high_resolution_clock::now();
            auto partial = std::chrono::high_resolution_clock::now();

            std::cout << "- Testing One norm, elapsed: ";

            for(std::size_t j = 0; j < tests; ++j)
                matrix.template norm<One>(); // Weird required syntax.

            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - partial).count() / 1E6 << " second(s)." << std::endl;

            std::cout << "- Testing Infinity norm, elapsed: ";
            partial = std::chrono::high_resolution_clock::now();

            for(std::size_t j = 0; j < tests; ++j)
                matrix.template norm<Infinity>(); // Weird required syntax.

            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - partial).count() / 1E6 << " second(s)." << std::endl;

            std::cout << "- Testing Frobenius norm, elapsed: ";
            partial = std::chrono::high_resolution_clock::now();

            for(std::size_t j = 0; j < tests; ++j)
                matrix.template norm<Frobenius>(); // Weird required syntax.

            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - partial).count() / 1E6 << " second(s)." << std::endl;

            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\nTested " << 3 * tests << " norms for a " << matrix.rows() << " by " << matrix.columns() << " sparse matrix." << std::endl;
            std::cout << "Matrix has a sparsity of " << matrix.sparsity() << ", it is " << (O == Row ? "row-first" : "column-first") << " ordered and it is " << (matrix.is_compressed() ? "in" : "not in") << " compressed form." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;
        }

    }

}

#endif