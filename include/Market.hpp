/**
 * @file Market.hpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MARKET_PACS
#define MARKET_PACS

// Type.
#include <Type.hpp>

// Filename.
#include <string>

// IO handling.
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// Matrix.
#include <Matrix.hpp>

namespace pacs {

    namespace algebra {
        
        /**
         * @brief Loads a Matrix written in market format.
         * 
         * @tparam T 
         * @tparam O 
         * @param filename 
         * @param verbose 
         * @return Matrix<T, O> 
         */
        template<MatrixType T, Order O = Row>
        Matrix<T, O> market(const std::string &filename, const bool &verbose = false) {
            std::map<std::array<std::size_t, 2>, double> elements;
            std::size_t rows, columns;

            // File loading.
            std::ifstream file{filename};
            std::string line;

            if(!(file))
                std::cerr << "Could not load a Matrix [" << filename << "]" << std::endl;

            // Skipping the first lines (comments).
            do { std::getline(file, line); } while(std::strcmp(&line[0], "%") == 0);

            // Matrix size.
            std::getline(file, line); // Matrix dimension.
            std::stringstream size{line}; // Matrix size.
            size >> rows >> columns;

            // Reads data.
            while(std::getline(file, line)) {
                std::size_t row, column;
                long double element;

                std::stringstream data{line};
                data >> row >> column >> element;

                if constexpr (O == Row) {
                    elements[{row - 1, column - 1}] = static_cast<T>(element);
                } else {
                    elements[{column - 1, row - 1}] = static_cast<T>(element);
                }
            }

            file.close();

            // Matrix.
            std::size_t first = O == Row ? rows : columns;
            std::size_t second = O == Row ? columns : rows;

            Matrix<T, O> matrix{first, second, elements};

            if(verbose)
                std::cerr << "Loaded a " << rows << " by " << columns << ", " << elements.size() << " elements Matrix [" << filename << "]\n" << matrix << std::endl;

            return matrix;
        }

        /**
         * @brief Dumps a Matrix to a market format file.
         * 
         * @tparam T 
         * @tparam O 
         * @param filename 
         * @param verbose 
         */
        template<MatrixType T, Order O = Row>
        void market(const Matrix<T, O> &matrix, const std::string &filename, const bool &verbose = false) {
            // File loading.
            std::ofstream file{filename};

            if(!(file))
                std::cerr << "Could not dump the Matrix [" << filename << "]" << std::endl;

            file << "%% Dumped matrix.\n";
            file << matrix.rows() << " " << matrix.columns() << " " << matrix.size() << "\n";

            if(!(matrix.is_compressed())) {

                std::map<std::array<std::size_t, 2>, T> elements = matrix.get_elements();

                for(const auto &[key, value]: elements) {
                    if constexpr (O == Row)
                        file << key[0] << " " << key[1] << " " << std::setprecision(12) << std::scientific << value << "\n";

                    if constexpr (O == Column)
                        file << key[1] << " " << key[0] << " " << std::setprecision(12) << std::scientific << value << "\n";
                }

            } else {
                
                std::vector<size_t> inner = matrix.get_inner();
                std::vector<size_t> outer = matrix.get_outer();
                std::vector<T> values = matrix.get_values();

                for(std::size_t j = 0; j < inner.size() - 1; ++j) {
                    for(std::size_t k = inner[j]; k < inner[j + 1]; ++k) {
                        if constexpr (O == Row)
                            file << j << " " << outer[k] << " " << std::setprecision(12) << std::scientific << values[k] << "\n";

                        if constexpr (O == Column)
                            file << outer[k] << " " << j << " " << std::setprecision(12) << std::scientific << values[k] << "\n";
                    }
                }

            }

            file.close();

            if(verbose)
                std::cerr << "Dumped a " << matrix.rows() << " by " << matrix.columns() << " Matrix [" << filename << "]\n" << std::endl;

        }

    }

}

#endif