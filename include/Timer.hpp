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
        void timer(const Matrix<T, O> &matrix, const std::vector<T> &vector, const std::size_t &tests = 1E6) {

            // Start.
            auto start = std::chrono::high_resolution_clock::now();

            for(std::size_t j = 0; j < tests; ++j)
                matrix * vector;

            // Stop.
            auto stop = std::chrono::high_resolution_clock::now();

            // Results.
            std::cout << "\n" << tests << " products." << std::endl;
            std::cout << matrix.rows() << " by " << matrix.columns() << " elements matrix times a " << vector.size() << " element(s) vector." << std::endl;
            std::cout << "Sparsity: " << matrix.sparsity() << "." << std::endl;
            std::cout << "Ordering: " << (O == Row ? "row-first" : "column-first") << "." << std::endl;
            std::cout << "Compression: " << (matrix.is_compressed() ? "yes" : "no") << "." << std::endl;
            std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 1E6 << " second(s)." << std::endl;

        }

    }

}

#endif