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

// Containers.
#include <vector>
#include <array>
#include <map>

// Output.
#include <iostream>

// Assertions.
#include <cassert>

// Math.
#include <complex>
#include <cmath>

// Algorithms.
#include <algorithm>
#include <execution>
#include <ranges>

// Zero tolerance.
#ifndef TOLERANCE_PACS
#define TOLERANCE_PACS 1E-8
#endif

// Zero checks.
#define ZCHECK_PACS

namespace pacs {

    namespace algebra {

        /**
         * @brief Default orderings.
         *
         */
        enum Order {Row, Column};

        /**
         * @brief Norms.
         *
         */
        enum Norm {One, Infinity, Frobenius};

        /**
         * @brief Sparse matrix class.
         *
         * @tparam T Matrix' type.
         * @tparam O Matrix' ordering.
         */
        template<typename T, Order O = Row>
        class Matrix {
            private:

                // Size (Rows x Columns or Columns x Rows).
                const std::size_t first; // First dimension.
                const std::size_t second; // Second dimension.

                // Compressed flag.
                bool compressed = false;

                // COOmap dynamic storage format.
                mutable std::map<std::array<std::size_t, 2>, T> elements;

                // CSR/CSC compressed storage format.
                std::vector<std::size_t> inner;
                std::vector<std::size_t> outer;
                std::vector<T> values;

            public:

                // CONSTRUCTORS.

                /**
                 * @brief Construct a new empty Matrix.
                 *
                 * @param first
                 * @param second
                 */
                Matrix(const std::size_t &first, const std::size_t &second): first{first}, second{second} {}

                /**
                 * @brief Construct a new Matrix from a given std::map.
                 *
                 * @param first
                 * @param second
                 * @param elements
                 */
                Matrix(const std::size_t &first, const std::size_t &second, const std::map<std::array<std::size_t, 2>, T> elements):
                first{first}, second{second}, elements{elements} {
                    #ifndef NDEBUG // Integrity checks.
                    assert((first > 0) && (second > 0));

                    // WIP.

                    #endif
                }

                /**
                 * @brief Construct a new Matrix from given inner, outer and values vectors.
                 *
                 * @param first
                 * @param second
                 * @param elements
                 */
                Matrix(const std::size_t &first, const std::size_t &second, const std::vector<std::size_t> &inner, const std::vector<std::size_t> &outer, const std::vector<T> &values):
                first{first}, second{second}, compressed{true}, inner{inner}, outer{outer}, values{values} {
                    #ifndef NDEBUG // Integrity checks.
                    assert((first > 0) && (second > 0));

                    // WIP.

                    #endif
                }

                /**
                 * @brief Copy constructor.
                 *
                 * @param matrix
                 */
                Matrix(const Matrix &matrix): first{matrix.first}, second{matrix.second}, compressed{matrix.compressed} {
                    if(!(matrix.compressed)) {
                        this->elements = matrix.elements;
                    } else {
                        this->inner = matrix.inner;
                        this->outer = matrix.outer;
                        this->values = matrix.values;
                    }
                }

                /**
                 * @brief Copies an existing matrix.
                 *
                 * @param matrix
                 * @return Matrix&
                 */
                Matrix &operator =(const Matrix &matrix) {
                    #ifndef NDEBUG
                    assert((this->first == matrix.first) && (this->second == matrix.second));
                    #endif

                    if(!(matrix.compressed)) {
                        this->compressed = false;
                        this->elements = matrix.elements;

                        this->inner.clear();
                        this->outer.clear();
                        this->values.clear();
                    } else {
                        this->compressed = true;
                        this->elements.clear();

                        this->inner = matrix.inner;
                        this->outer = matrix.outer;
                        this->values = matrix.values;
                    }

                    return *this;
                }

                // CALL OPERATORS.

                /**
                 * @brief Const call operator, returns the (i, j)-th element.
                 *
                 * @param j
                 * @param k
                 * @return T
                 */
                T operator ()(const std::size_t &j, const std::size_t &k) const {
                    #ifndef NDEBUG // Out-of-bound check.
                    assert((j < first) && (k < second));
                    #endif

                    // Checks for the value inside elements, otherwise returns static_cast<T>(0).
                    if(!(this->compressed))
                        return this->elements.contains({j, k}) ? this->elements[{j, k}] : static_cast<T>(0);

                    // Looks for the value.
                    for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i) {
                        if(k == this->outer[i])
                            return this->values[i];
                    }

                    // Default return.
                    return static_cast<T>(0);
                }

                /**
                 * @brief Call operator, returns a reference to the (i, j)-th element on uncompressed Matrix.
                 *
                 * @param j
                 * @param k
                 * @return T&
                 */
                T &operator ()(const std::size_t &j, const std::size_t &k) {
                    #ifndef NDEBUG // Out-of-bound and (un)compression check.
                    assert((j < first) && (k < second));
                    assert(!(this->compressed));
                    #endif

                    return this->elements[{j, k}];
                }

                // SHAPE.

                /**
                 * @brief Returns the number of rows.
                 *
                 * @return constexpr std::size_t
                 */
                constexpr std::size_t rows() const {
                    if constexpr (O == Row)
                        return this->first;

                    return this->second;
                }

                /**
                 * @brief Returns the number of columns.
                 *
                 * @return constexpr std::size_t
                 */
                constexpr std::size_t columns() const {
                    if constexpr (O == Column)
                        return this->first;

                    return this->second;
                }

                /**
                 * @brief Returns the matrix' shape: Rows x Columns.
                 *
                 * @return constexpr std::pair<std::size_t, std::size_t>
                 */
                constexpr std::pair<std::size_t, std::size_t> shape() const {
                    return {this->rows(), this->columns()};
                }

                /**
                 * @brief Returns a reshaped matrix in the same (un)compressed state.
                 *
                 * @param first
                 * @param second
                 * @return Matrix
                 */
                Matrix reshape(const std::size_t &first, const std::size_t &second, const bool &compress = false) const {
                    if(!(this->compressed)) {
                        Matrix matrix{first, second, this->elements};

                        if(compress)
                            matrix.compress();

                        return matrix;
                    }

                    Matrix matrix{first, second, this->inner, this->outer, this->values};

                    if(compress)
                        return matrix;

                    matrix.uncompress();
                    return matrix;
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
                        for(auto it = this->elements.lower_bound(current); (*it).first < (*(this->elements.lower_bound(next))).first; ++it) {
                            auto [key, value] = (*it);

                            #ifdef ZCHECK_PACS // std::abs : T -> floating_point must be defined for this to work.
                                if(std::abs(value) > TOLERANCE_PACS) {
                                    this->outer.emplace_back(key[1]);
                                    this->values.emplace_back(value);
                                    ++index;
                                }
                            #else
                                this->outer.emplace_back(key[1]);
                                this->values.emplace_back(value);
                                ++index;
                            #endif
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
                inline bool is_compressed() const {
                    return this->compressed;
                }

                // OPERATIONS.

                /**
                 * @brief Returns the product of Matrix x Vector.
                 *
                 * @param vector
                 * @return std::vector<T>
                 */
                std::vector<T> operator *(const std::vector<T> &vector) const {
                    #ifndef NDEBUG // Vector size check.
                    assert(vector.size() == this->columns());
                    #endif

                    std::vector<T> result;
                    result.resize(this->rows(), static_cast<T>(0));

                    // Standard Row x Column product.
                    if constexpr (O == Row) {
                        if(!(this->compressed)) { // Slower.

                            // Full iteration on non-zero elements.
                            for(const auto &[key, value]: this->elements)
                                result[key[0]] += value * vector[key[1]];

                        } else { // Faster.

                            // Standard product.
                            for(std::size_t j = 0; j < result.size(); ++j) {
                                for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i)
                                    result[j] += this->values[i] * vector[this->outer[i]];
                            }
                        }
                    }

                    if constexpr (O == Column) {
                        if(!(this->compressed)) { // Slower.
                            
                            // Full iteration on non-zero elements.
                            for(const auto &[key, value]: this->elements)
                                result[key[1]] += value * vector[key[0]];

                        } else { // Faster.

                            // Linear combination of columns.
                            for(std::size_t j = 0; j < result.size(); ++j) {
                                for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i)
                                    result[this->outer[i]] += this->values[i] * vector[j];
                            }
                        }
                    }

                    return result;
                }

                /**
                 * @brief Returns the product of Vector x Matrix.
                 *
                 * @param vector
                 * @param matrix
                 * @return std::vector<T>
                 */
                friend std::vector<T> operator *(const std::vector<T> &vector, const Matrix &matrix) {
                    #ifndef NDEBUG // Vector size check.
                    assert(vector.size() == matrix.rows());
                    #endif

                    std::vector<T> result;
                    result.resize(matrix.columns(), static_cast<T>(0));

                    // Standard Row x Column product.
                    if constexpr (O == Column) {
                        if(!(matrix.compressed)) { // Slower.

                            // Full iteration on non-zero elements.
                            for(const auto &[key, value]: matrix.elements)
                                result[key[0]] += vector[key[1]] * value;

                        } else { // Faster.

                            // Standard product.
                            for(std::size_t j = 0; j < result.size(); ++j) {
                                for(std::size_t i = matrix.inner[j]; i < matrix.inner[j + 1]; ++i)
                                    result[j] += vector[matrix.outer[i]] * matrix.values[i];
                            }
                        }
                    }

                    if constexpr (O == Row) {
                        if(!(matrix.compressed)) { // Slower.

                            // Full iteration on non-zero elements.
                            for(const auto &[key, value]: matrix.elements)
                                result[key[1]] += vector[key[0]] * value;

                        } else { // Faster.

                            // Linear combination of rows.
                            for(std::size_t j = 0; j < result.size(); ++j) {
                                for(std::size_t i = matrix.inner[j]; i < matrix.inner[j + 1]; ++i)
                                    result[matrix.outer[i]] += vector[j] * matrix.values[i];
                            }
                        }
                    }

                    return result;
                }

                /**
                 * @brief Returns the product of Matrix x Matrix (same ordering).
                 * 
                 * @param matrix 
                 * @return Matrix 
                 */
                Matrix operator *(const Matrix &matrix) const {
                    #ifndef NDEBUG
                    assert(this->columns() == matrix.rows());
                    #endif
                    
                    // Result.
                    std::map<std::array<std::size_t, 2>, T> elements;

                    if constexpr (O == Row) {
                        
                        if(this->compressed) {
                            if(matrix.compressed) {
                                
                                // Iteration through this' rows.
                                for(std::size_t j = 0; j < this->rows(); ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(matrix.columns(), static_cast<T>(0));

                                    for(std::size_t h = this->inner[j]; h < this->inner[j + 1]; ++h)
                                        row[this->outer[h]] = this->values[h];

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(this->columns(), static_cast<T>(0));

                                    // Linear combination of matrix' rows.
                                    for(std::size_t k = 0; k < matrix.rows(); ++k) { // k-th row of matrix.
                                        for(std::size_t h = matrix.inner[k]; h < matrix.inner[k + 1]; ++h) {
                                            product[matrix.outer[h]] += row[k] * matrix.values[h];
                                        }
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS)
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            } else {

                                // Iteration through this' rows.
                                for(std::size_t j = 0; j < this->rows(); ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(matrix.columns(), static_cast<T>(0));

                                    for(std::size_t h = this->inner[j]; h < this->inner[j + 1]; ++h)
                                        row[this->outer[h]] = this->values[h];

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(this->columns(), static_cast<T>(0));

                                    // Full iteration on matrix' non-zero elements.
                                    for(const auto &[key, value]: matrix.elements)
                                        product[key[1]] += row[key[0]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS)
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            }
                        } else {
                            if(matrix.compressed) {
                                
                                // Iteration through this' rows.
                                for(std::size_t j = 0; j < this->rows() - 1; ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(matrix.columns(), static_cast<T>(0));

                                    for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*(this->elements.lower_bound({j + 1, 0}))).first; ++it)
                                        row[(*it).first[1]] = (*it).second;

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(this->columns(), static_cast<T>(0));

                                    // Linear combination of matrix' rows.
                                    for(std::size_t k = 0; k < matrix.rows(); ++k) { // k-th row of matrix.
                                        for(std::size_t h = matrix.inner[k]; h < matrix.inner[k + 1]; ++h) {
                                            product[matrix.outer[h]] += row[k] * matrix.values[h];
                                        }
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS)
                                            elements[{j, h}] = product[h];
                                    }
                                }

                                // Last row.
                                std::size_t j = this->rows() - 1;

                                // Row extraction from this.
                                std::vector<T> row;
                                row.resize(matrix.columns(), static_cast<T>(0));

                                for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*(--this->elements.end())).first; ++it)
                                    row[(*it).first[1]] = (*it).second;

                                if(this->elements.contains({j, this->columns() - 1}))
                                    row[this->columns() - 1] = this->elements[{j, this->columns() - 1}];

                                // Result's row.
                                std::vector<T> product;
                                product.resize(this->columns(), static_cast<T>(0));

                                // Linear combination of matrix' rows.
                                for(std::size_t k = 0; k < matrix.rows(); ++k) { // k-th row of matrix.
                                    for(std::size_t h = matrix.inner[k]; h < matrix.inner[k + 1]; ++h) {
                                        product[matrix.outer[h]] += row[k] * matrix.values[h];
                                    }
                                }

                                // Updates elements.
                                for(std::size_t h = 0; h < product.size(); ++h) {
                                    if(std::abs(product[h]) > TOLERANCE_PACS)
                                        elements[{j, h}] = product[h];
                                }

                            } else {

                                // Iteration through this' rows.
                                for(std::size_t j = 0; j < this->rows() - 1; ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(matrix.columns(), static_cast<T>(0));

                                    for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*(this->elements.lower_bound({j + 1, 0}))).first; ++it)
                                        row[(*it).first[1]] = (*it).second;

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(this->columns(), static_cast<T>(0));

                                    // Full iteration on matrix' non-zero elements.
                                    for(const auto &[key, value]: matrix.elements)
                                        product[key[1]] += row[key[0]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS)
                                            elements[{j, h}] = product[h];
                                    }
                                }

                                // Last row.
                                std::size_t j = this->rows() - 1;

                                // Row extraction from this.
                                std::vector<T> row;
                                row.resize(matrix.columns(), static_cast<T>(0));

                                for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*(--this->elements.end())).first; ++it)
                                    row[(*it).first[1]] = (*it).second;

                                // Last element.
                                if(this->elements.contains({j, this->columns() - 1}))
                                    row[this->columns() - 1] = this->elements[{j, this->columns() - 1}];

                                // Result's row.
                                std::vector<T> product;
                                product.resize(this->columns(), static_cast<T>(0));

                                // Full iteration on matrix' non-zero elements.
                                for(const auto &[key, value]: matrix.elements)
                                    product[key[1]] += row[key[0]] * value;

                                // Updates elements.
                                for(std::size_t h = 0; h < product.size(); ++h) {
                                    if(std::abs(product[h]) > TOLERANCE_PACS)
                                        elements[{j, h}] = product[h];
                                }

                            }
                        }

                    }

                    if constexpr (O == Column) {
                        
                        // WIP.

                    }

                    return Matrix{this->rows(), matrix.columns(), elements};
                }

                // NORM.

                /**
                 * @brief Returns a norm for the Matrix.
                 *
                 * @tparam N
                 * @return double
                 */
                template<Norm N = Frobenius>
                double norm() const {
                    #ifdef PARALLEL_PACS
                        auto absolute = [](const double &x) { return std::abs(x); };
                    #endif

                    double norm = 0.0, max = 0.0;

                    // On the secondary direction.
                    if constexpr (N == One) {

                        if(!(this->compressed)) {

                            std::vector<double> sums;
                            sums.resize(this->second, 0.0);

                            for(const auto &[key, value]: this->elements)
                                sums[key[1]] += std::abs(value);

                            norm = std::ranges::max(sums);

                        } else {

                            std::vector<double> sums;
                            sums.resize(this->second, 0.0);

                            for(std::size_t j = 0; j < this->values.size(); ++j)
                                sums[this->outer[j]] += std::abs(this->values[j]);

                            norm = std::ranges::max(sums);

                        }

                    }

                    // On the primary direction.
                    if constexpr (N == Infinity) {

                        if(!(this->compressed)) {

                            std::vector<double> sums;
                            sums.resize(this->first, 0.0);

                            for(const auto &[key, value]: this->elements)
                                sums[key[0]] += std::abs(value);

                            norm = std::ranges::max(sums);

                        } else {

                            for(std::size_t j = 0; j < this->inner.size() - 1; ++j) {
                                #ifdef PARALLEL_PACS
                                    double sum = std::transform_reduce(std::execution::par, this->values.begin() + this->inner[j], this->values.begin() + this->inner[j + 1], 0.0, std::plus{}, absolute);
                                #else
                                    double sum = 0.0;

                                    for(std::size_t k = this->inner[j]; k < this->inner[j + 1]; ++k)
                                        sum += this->values[k];
                                #endif

                                max = sum > max ? sum : max;
                            }

                            norm = max;

                        }

                    }

                    if constexpr (N == Frobenius) {

                        if(!(this->compressed)) {
                            for(const auto &[key, value]: this->elements)
                                norm += static_cast<double>(std::abs(value) * std::abs(value));

                            norm = std::sqrt(norm);
                        } else {

                            #ifdef PARALLEL_PACS
                                norm = std::sqrt(std::transform_reduce(std::execution::par, this->values.begin(), this->values.end(), 0.0, std::plus{}, absolute));
                            #else
                                for(const auto &value: this->values)
                                    norm += static_cast<double>(std::abs(value) * std::abs(value));
                                
                                norm = std::sqrt(norm);
                            #endif

                        }

                    }

                    return norm;
                }

                // METHODS.

                /**
                 * @brief Returns the 'sparsity' of the Matrix.
                 *
                 * @return double
                 */
                inline double sparsity() const {
                    if(!(this->compressed))
                        return static_cast<double>(this->elements.size()) / static_cast<double>(this->first * this->second);

                    return static_cast<double>(this->values.size()) / static_cast<double>(this->first * this->second);
                }

                /**
                 * @brief Returns the 'density' of the Matrix.
                 *
                 * @return double
                 */
                inline double density() const {
                    return 1.0 - this->sparsity();
                }

                // OUTPUT.

                /**
                 * @brief Matrix output.
                 *
                 * @param ost
                 * @param matrix
                 * @return std::ostream&
                 */
                friend std::ostream &operator <<(std::ostream &ost, const Matrix &matrix) {
                    if(!(matrix.compressed)) {
                        for(const auto &[key, value]: matrix.elements) {
                            ost << "(" << key[0] << ", " << key[1] << "): " << value;

                            if(key != (*--matrix.elements.end()).first)
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