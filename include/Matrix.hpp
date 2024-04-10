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

// Maps.
#include <map>

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

                // CSC/CSR compressed storage formate.

                // WIP.

            public:

                // CONSTRUCTORS.

                // WIP.

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

                    if constexpr (O == Row)
                        assert((j < first) && (k < second));

                    if constexpr (O == Column)
                        assert((j < second) && (k < first));

                    #endif

                    // Returns the value if present, otherwise T{0}.
                    if(!(this->compressed))
                        return this->elements.contains({j, k}) ? this->elements[{j, k}] : T{0};

                    // WIP.

                    return T{0};
                }

                /**
                 * @brief Call operator.
                 * 
                 * @param j 
                 * @param k 
                 * @return T& 
                 */
                T &operator () (const std::size_t &j, const std::size_t &k) {
                    // WIP.

                    return T{0};
                }

                // COMPRESSION.

                /**
                 * @brief Compress an uncompressed matrix.
                 * 
                 */
                void compress() {
                    if(this->compressed)
                        return;

                    this->compressed = true;

                    // WIP.
                }

                /**
                 * @brief Uncompress a compressed matrix.
                 * 
                 */
                void uncompress() {
                    if(!(this->compressed))
                        return;

                    this->compressed = false;

                    // WIP.
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
        };

    }

}

#endif