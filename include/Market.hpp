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

// Filename.
#include <string>

// IO handling.
#include <iostream>
#include <fstream>
#include <sstream>

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
        template<typename T, Order O>
        Matrix<T, O> market(const std::string &filename, const bool &verbose = false) {
            std::map<std::array<std::size_t, 2>, double> elements;
            std::size_t rows, columns;

            // File loading.
            std::ifstream file{filename};
            std::string line;

            if(!(file))
                std::cerr << "Could not load a Matrix [" << filename << "]" << std::endl;

            // Matrix size.
            std::getline(file, line); // Useless info.
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
                    elements[{row, column}] = static_cast<T>(element);
                } else {
                    elements[{column, row}] = static_cast<T>(element);
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

    }

}

#endif