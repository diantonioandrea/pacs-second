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

// Matrix.
#include <Matrix.hpp>

namespace pacs {

    namespace algebra {

        Matrix<double, Row> market(const std::string &, const bool &);
        Matrix<double, Row> market(const std::string &);

    }

}

#endif