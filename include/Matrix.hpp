/**
 * @file Matrix.hpp
 * @author Andrea Di Antonio (github.com/diantonioandrea)
 * @brief 
 * @date 2024-04-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MATRIX_PACS
#define MATRIX_PACS

// Maps and Vectors.
#include <vector>
#include <map>

// Output.
#include <iostream>

// Assertions.
#include <cassert>

namespace pacs {
    
    namespace algebra {
        
        /**
         * @brief Default orderings.
         * 
         */
        enum Order {Row, Column};

        /**
         * @brief Matrix class.
         * 
         * @tparam T 
         * @tparam O 
         */
        template<typename T, Order O = Row>
        class Matrix {
            private:

                // Size (Row x Columns or Columns x Row).
                const std::size_t first; // First dimension.
                const std::size_t second; // Second dimension.

                // Compressed flag.
                bool compressed = false;

                // COOmap dynamic storage format.
                mutable std::map<std::array<std::size_t, 2>, T> elements;

                // CSR/CSC compressed storage formate.
                std::vector<std::size_t> inner;
                std::vector<std::size_t> outer;
                std::vector<T> values;

            public:

                // CONSTRUCTORS.

                /**
                 * @brief Construct a new Matrix< T,  O> from a given std::map.
                 * 
                 * @param first 
                 * @param second 
                 * @param elements 
                 */
                Matrix<T, O>(const std::size_t &first, const std::size_t &second, const std::map<std::array<std::size_t, 2>, T> elements): 
                first{first}, second{second} {
                    this->elements = elements;
                }

                // CALL OPERATORS.

                /**
                 * @brief Const call operator.
                 * 
                 * @param j 
                 * @param k 
                 * @return T 
                 */
                T operator ()(const std::size_t &j, const std::size_t &k) const {
                    #ifndef NDEBUG // Out-of-bound check.
                    assert((j < first) && (k < second));
                    #endif

                    // Returns the value if present, otherwise static_cast<T>(0).
                    if(!(this->compressed))
                        return this->elements.contains({j, k}) ? this->elements[{j, k}] : static_cast<T>(0);

                    for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i) {
                        if(k == this->outer[i])
                            return this->values[i];
                    }

                    return static_cast<T>(0);
                }

                /**
                 * @brief Call operator.
                 * 
                 * @param j 
                 * @param k 
                 * @return T& 
                 */
                // T &operator ()(const std::size_t &j, const std::size_t &k) {
                //     #ifndef NDEBUG // Out-of-bound check.
                //     assert((j < first) && (k < second));
                //     assert(!(this->compressed));
                //     #endif

                //     return this->elements[{j, k}];
                // }

                // SHAPE.

                /**
                 * @brief Returns the number of rows.
                 * 
                 * @return std::size_t 
                 */
                constexpr std::size_t rows() const {
                    if constexpr (O == Row)
                        return this->first;

                    return this->second;
                }

                /**
                 * @brief Returns the number of columns.
                 * 
                 * @return std::size_t 
                 */
                constexpr std::size_t columns() const {
                    if constexpr (O == Column)
                        return this->first;

                    return this->second;
                }

                // COMPRESSION.

                /**
                 * @brief Compresses an uncompressed matrix.
                 * 
                 */
                void compress() {
                    if(this->compressed)
                        return;

                    std::size_t index = 0;
                    std::array<std::size_t, 2> current{0, 0};
                    std::array<std::size_t, 2> next{1, 0};

                    this->inner.resize(this->first + 1);
                    this->inner[0] = index;

                    // Compression.
                    for(std::size_t j = 1; j < this->first + 1; ++j) {
                        for(auto it = this->elements.lower_bound(current); (*it).first < (*(this->elements.upper_bound(next))).first; ++it) {
                            if((*it).second != static_cast<T>(0)) {
                                this->outer.emplace_back((*it).first[1]);
                                this->values.emplace_back((*it).second);
                                ++index;
                            }
                        }

                        this->inner[j] = index;
                        ++current[0];
                        ++next[0];
                    }
                    
                    this->compressed = true;
                    this->elements.clear();                
                }

                /**
                 * @brief Uncompresses a compressed matrix.
                 * 
                 */
                void uncompress() {
                    if(!(this->compressed))
                        return;

                    // Uncompression.
                    for(std::size_t j = 0; j < this->inner.size() - 1; ++j) {
                        for(std::size_t k = this->inner[j]; k < this->inner[j + 1]; ++k) {
                            this->elements[{j, this->outer[k]}] = this->values[k];
                        }
                    }

                    this->compressed = false;
                    this->inner.clear();
                    this->outer.clear();
                    this->values.clear();            
                }

                /**
                 * @brief Returns the compressed state.
                 * 
                 * @return true 
                 * @return false 
                 */
                bool is_compressed() const {
                    return this->compressed;
                }

                // OUTPUT.

                /**
                 * @brief Matrix output.
                 * 
                 * @param ost 
                 * @param matrix 
                 * @return std::ostream& 
                 */
                friend std::ostream &operator <<(std::ostream &ost, const Matrix<T, O> &matrix) {
                    if(!(matrix.compressed)) {
                        for(const auto &[position, value]: matrix.elements) {
                            ost << "(" << position[0] << ", " << position[1] << "): " << value;

                            if(position != (*--matrix.elements.end()).first)
                                ost << std::endl;
                        }

                    } else {
                        ost << "Inner: ";

                        for(const auto &value: matrix.inner)
                            ost << value << " ";

                        ost << std::endl << "Outer: ";

                        for(const auto &value: matrix.outer)
                            ost << value << " ";
                        
                        ost << std::endl <<  "Values: ";

                        for(const auto &value: matrix.values)
                            ost << value << " ";
                    }

                    return ost;
                }
        };

    }

}

#endif