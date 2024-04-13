/**
 * @file Market.cpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// Input-output handling.
#include <fstream>
#include <iostream>
#include <sstream>

// Containers.
#include <map>
#include <array>

#include <Market.hpp>

namespace pacs {

    namespace algebra {
        
        /**
         * @brief Loads a row-first real Matrix written in market format.
         * 
         * @param filename 
         * @return Matrix<double, Row> 
         */
        Matrix<double, Row> market(const std::string &filename, const bool &verbose) {
            std::map<std::array<std::size_t, 2>, double> elements;
            std::size_t rows, columns;

            // File loading.
            std::ifstream file{filename};
            std::string line;

            if(!(file))
                std::cerr << "Could not load a Matrix [" << filename << "]" << std::endl;

            // Matrix size.
            std::getline(file, line); // Useless info.
            std::getline(file, line);

            std::stringstream size{line}; // Matrix size.
            size >> rows >> columns;

            // Reads data.
            while(std::getline(file, line)) {
                std::size_t first, second;
                double element;

                std::stringstream data{line};
                data >> first >> second >> element;

                elements[{first, second}] = element;
            }

            file.close();

            // Matrix.
            Matrix<double, Row> matrix{rows, columns, elements};

            if(verbose)
                std::cerr << "Loaded a " << rows << " by " << columns << ", " << elements.size() << " elements Matrix [" << filename << "]\n" << matrix << std::endl;

            return matrix;
        }

        /**
         * @brief Loads a row-first real Matrix written in market format with no verbosity.
         * 
         * @param filename 
         * @return Matrix<double, Row> 
         */
        Matrix<double, Row> market(const std::string &filename) {
            return market(filename, false);
        }

    }

}