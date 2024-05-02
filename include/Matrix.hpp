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

// Type.
#include <Type.hpp>

// Containers.
#include <vector>
#include <array>
#include <map>

// Output.
#include <iostream>

// Assertions.
#include <cassert>

// Algorithms.
#include <algorithm>
#include <execution>
#include <ranges>

// Math.
#include <cmath>

// Zero tolerance.
#ifndef TOLERANCE_PACS
#define TOLERANCE_PACS 1E-10
#endif

namespace pacs {

    namespace algebra {

        /**
         * @brief Sparse matrix class.
         *
         * @tparam T Matrix' type.
         * @tparam O Matrix' ordering.
         */
        template<MatrixType T, Order O = Row>
        class Matrix {
            private:

                // Size (Rows by Columns or Columns by Rows).
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
                Matrix(const std::size_t &first, const std::size_t &second): first{first}, second{second} {
                    #ifndef NDEBUG // Integrity check.
                    assert((first > 0) && (second > 0));
                    #endif
                }

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

                    for(const auto &[key, value]: elements)
                        assert((key[0] < first) && (key[1] < second));

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

                    assert(inner.size() == first + 1);
                    for(std::size_t j = 1; j < inner.size(); ++j) {
                        assert(inner[j - 1] < values.size());
                        assert(inner[j] < values.size());
                        assert(inner[j - 1] <= inner[j]);
                    }

                    assert(outer.size() = values.size());
                    for(std::size_t j = 1; j < outer.size(); ++j) {
                        assert(outer[j - 1] < second);
                        assert(outer[j] < second);
                        assert(outer[j - 1] < outer[j]);
                    }

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

                    this->compressed = matrix.compressed;

                    if(!(matrix.compressed)) {
                        this->elements = matrix.elements;

                        this->inner.clear();
                        this->outer.clear();
                        this->values.clear();
                    } else {
                        this->elements.clear();

                        this->inner = matrix.inner;
                        this->outer = matrix.outer;
                        this->values = matrix.values;
                    }

                    return *this;
                }

                // CONVERSION.

                /**
                 * @brief Converts a row or column Matrix to a Vector.
                 * 
                 * @return std::vector<T> 
                 */
                operator std::vector<T>() const {
                    #ifndef NDEBUG
                    assert((this->first == 1) || (this->second == 1));
                    #endif

                    std::vector<T> vector;
                    vector.resize(this->first * this->second, static_cast<T>(0));

                    if(!(this->compressed)) {

                        // Either key[0] or key[1] is always zero.
                        for(const auto &[key, value]: this->elements)
                            vector[key[1]] = value;

                    } else {

                        // Either j or outer[k] is always zero.
                        for(std::size_t j = 0; j < this->inner.size() - 1; ++j) {
                            for(std::size_t k = this->inner[j]; k < this->inner[j + 1]; ++k)
                                vector[j + this->outer[k]] = this->values[k];
                        }

                    }

                    return vector;
                }

                // CALL OPERATORS.

                /**
                 * @brief Const call operator, returns the (i, j)-th element if present.
                 *
                 * @param j
                 * @param k
                 * @return T
                 */
                T operator ()(const std::size_t &j, const std::size_t &k) const {
                    #ifndef NDEBUG // Out-of-bound check.
                    assert((j < this->first) && (k < this->second));
                    #endif

                    // Checks for the value inside elements, otherwise returns static_cast<T>(0).
                    if(!(this->compressed))
                        return this->elements.contains({j, k}) ? this->elements[{j, k}] : static_cast<T>(0);

                    // Looks for the value on compressed Matrix.
                    for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i) {
                        if(k == this->outer[i])
                            return this->values[i];
                    }

                    // Default return.
                    return static_cast<T>(0);
                }

                // INSERTION.

                /**
                 * @brief Insert a new element.
                 * 
                 * @param j 
                 * @param k 
                 * @param element 
                 */
                void insert(const std::size_t &j, const std::size_t &k, const T &element) {
                    #ifndef NDEBUG // Out-of-bound and uncompression check.
                    assert((j < first) && (k < second));
                    assert(!(this->compressed));
                    #endif

                    #ifndef NDEBUG // Separate check not needed.
                    if(std::abs(element) > TOLERANCE_PACS)
                        this->elements[{j, k}] = element;
                    #else
                    this->elements[{j, k}] = element;
                    #endif
                }

                /**
                 * @brief Inserts a vector of new elements.
                 * 
                 * @param coordinates 
                 * @param elements 
                 */
                void insert_vector(const std::vector<std::array<std::size_t, 2> > &coordinates, const std::vector<T> &elements) {
                    #ifndef NDEBUG // Out-of-bound and uncompression check.
                    assert(!(this->compressed));
                    assert(coordinates.size() == elements.size());
                    #endif

                    for(std::size_t j = 0; j < coordinates.size(); ++j) {

                        #ifndef NDEBUG
                        assert(coordinates[j][0] < this->first);
                        assert(coordinates[j][1] < this->second);

                        if(std::abs(elements[j]) > TOLERANCE_PACS)
                            this->elements[coordinates[j]] = elements[j];
                        #else
                        this->elements[coordinates[j]] = elements[j];
                        #endif

                    }
                }

                /**
                 * @brief Inserts a range of new elements.
                 * 
                 * @param start 
                 * @param end 
                 * @param elements 
                 */
                void insert_range(const std::array<std::size_t, 2> &start, const std::array<std::size_t, 2> &end, const std::vector<T> &elements) {
                    #ifndef NDEBUG // Out-of-bound and uncompression check.
                    assert(!(this->compressed));
                    assert((start[0] < end[0]) && (end[0] < this->first));
                    assert((start[1] < end[1]) && (end[1] < this->second));
                    assert((end[1] - start[1]) * (end[0] - start[0]) == elements.size());
                    #endif

                    for(std::size_t j = start[0]; j < end[0]; ++j) {
                        for(std::size_t k = start[1]; k < end[1]; ++k) {

                            #ifndef NDEBUG
                            if(std::abs(elements[j]) > TOLERANCE_PACS)
                                this->elements[{j, k}] = elements[j * (end[1] - start[1]) + k];
                            #else
                            this->elements[{j, k}] = elements[j * (end[1] - start[1]) + k];
                            #endif

                        }
                    }
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
                 * @brief Returns the reshaped matrix.
                 * 
                 * @param first 
                 * @param second 
                 * @return Matrix 
                 */
                Matrix reshape(const std::size_t &first, const std::size_t &second) const {
                    if(!(this->compressed))
                        return Matrix{first, second, this->elements};

                    return Matrix{first, second, this->inner, this->outer, this->values};
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

                            #ifndef NDEBUG
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

                            #ifndef NDEBUG
                            if(std::abs(this->values[k]) > TOLERANCE_PACS)
                                this->elements[{j, this->outer[k]}] = this->values[k];
                            #else
                            this->elements[{j, this->outer[k]}] = this->values[k];
                            #endif

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
                 * @brief Returns the product of Matrix x Scalar.
                 * 
                 * @param scalar 
                 * @return Matrix 
                 */
                Matrix operator *(const T &scalar) const {
                    #ifdef PARALLEL_PACS
                    auto product = [scalar](T element) { return element *= scalar; };
                    #endif

                    Matrix result = *this;

                    if(!(result.compressed)) {
                        for(const auto &[key, value]: result.elements) // Extremely slow.
                            result.elements[key] *= scalar;
                    } else {
                        
                        #ifdef PARALLEL_PACS // Actually faster.
                        std::transform(std::execution::par, result.values.begin(), result.values.end(), result.values.begin(), product);
                        #else
                        for(auto &value: result.values)
                            value *= scalar;
                        #endif
                    }

                    return result;
                }

                /**
                 * @brief Returns the product and assignment of Matrix *= Scalar.
                 * 
                 * @param scalar 
                 * @return Matrix& 
                 */
                Matrix &operator *=(const T &scalar) {
                    #ifdef PARALLEL_PACS
                    auto product = [scalar](T element) { return element *= scalar; };
                    #endif

                    if(!(this->compressed)) {
                        for(const auto &[key, value]: this->elements) // Extremely slow.
                            this->elements[key] *= scalar;
                    } else {
                        
                        #ifdef PARALLEL_PACS // Actually faster.
                        std::transform(std::execution::par, this->values.begin(), this->values.end(), this->values.begin(), product);
                        #else
                        for(auto &value: this->values)
                            value *= scalar;
                        #endif

                    }

                    return *this;
                }

                /**
                 * @brief Returns the division of Matrix / Scalar.
                 * 
                 * @param scalar 
                 * @return Matrix 
                 */
                Matrix operator /(const T &scalar) const {
                    #ifdef PARALLEL_PACS
                    auto division = [scalar](T element) { return element /= scalar; };
                    #endif

                    Matrix result = *this;

                    if(!(result.compressed)) {
                        for(const auto &[key, value]: result.elements) // Extremely slow.
                            result.elements[key] /= scalar;
                    } else {
                        
                        #ifdef PARALLEL_PACS // Actually faster.
                        std::transform(std::execution::par, result.values.begin(), result.values.end(), result.values.begin(), division);
                        #else
                        for(auto &value: result.values)
                            value /= scalar;
                        #endif

                    }

                    return result;
                }
                
                /**
                 * @brief Returns the division and assignment of Matrix /= Scalar.
                 * 
                 * @param scalar 
                 * @return Matrix& 
                 */
                Matrix &operator /=(const T &scalar) {
                    #ifdef PARALLEL_PACS
                    auto division = [scalar](T element) { return element *= scalar; };
                    #endif

                    if(!(this->compressed)) {
                        for(const auto &[key, value]: this->elements) // Extremely slow.
                            this->elements[key] /= scalar;
                    } else {
                        
                        #ifdef PARALLEL_PACS // Actually faster.
                        std::transform(std::execution::par, this->values.begin(), this->values.end(), this->values.begin(), division);
                        #else
                        for(auto &value: this->values)
                            value /= scalar;
                        #endif

                    }

                    return *this;
                }

                /**
                 * @brief Returns the product of Scalar x Matrix.
                 * 
                 * @tparam T 
                 * @param scalar 
                 * @return Matrix 
                 */
                friend Matrix operator *(const T &scalar, const Matrix &matrix) {
                    return matrix * scalar;
                }

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
                                    row.resize(this->columns(), static_cast<T>(0));

                                    for(std::size_t h = this->inner[j]; h < this->inner[j + 1]; ++h)
                                        row[this->outer[h]] = this->values[h];

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(matrix.columns(), static_cast<T>(0));

                                    // Linear combination of matrix' rows.
                                    for(std::size_t k = 0; k < matrix.rows(); ++k) { // k-th row of matrix.
                                        for(std::size_t h = matrix.inner[k]; h < matrix.inner[k + 1]; ++h) {
                                            product[matrix.outer[h]] += row[k] * matrix.values[h];
                                        }
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            } else {

                                // Iteration through this' rows.
                                for(std::size_t j = 0; j < this->rows(); ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(this->columns(), static_cast<T>(0));

                                    for(std::size_t h = this->inner[j]; h < this->inner[j + 1]; ++h)
                                        row[this->outer[h]] = this->values[h];

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(matrix.columns(), static_cast<T>(0));

                                    // Full iteration on matrix' non-zero elements.
                                    for(const auto &[key, value]: matrix.elements)
                                        product[key[1]] += row[key[0]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            }
                        } else {
                            if(matrix.compressed) {

                                // Iteration through this' rows.
                                auto stop = (*(--this->elements.end())).first;
                                for(std::size_t j = 0; j < this->rows() - 1; ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(this->columns(), static_cast<T>(0));

                                    for(auto it = this->elements.lower_bound({j, 0}); (*it).first < std::array<std::size_t, 2>{j + 1, 0}; ++it) {
                                        row[(*it).first[1]] = (*it).second;

                                        if((*it).first == stop) // May cause overhead.
                                            break;
                                    }

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(matrix.columns(), static_cast<T>(0));

                                    // Linear combination of matrix' rows.
                                    for(std::size_t k = 0; k < matrix.rows(); ++k) { // k-th row of matrix.
                                        for(std::size_t h = matrix.inner[k]; h < matrix.inner[k + 1]; ++h) {
                                            product[matrix.outer[h]] += row[k] * matrix.values[h];
                                        }
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            } else {

                                // Iteration through this' rows.
                                auto stop = (*(--this->elements.end())).first;
                                for(std::size_t j = 0; j < this->rows(); ++j) { // j-th row of this.

                                    // Row extraction from this.
                                    std::vector<T> row;
                                    row.resize(this->columns(), static_cast<T>(0));

                                    for(auto it = this->elements.lower_bound({j, 0}); (*it).first < std::array<std::size_t, 2>{j + 1, 0}; ++it) {
                                        row[(*it).first[1]] = (*it).second;

                                        if((*it).first == stop) // May cause overhead.
                                            break;
                                    }

                                    // Result's row.
                                    std::vector<T> product;
                                    product.resize(matrix.columns(), static_cast<T>(0));

                                    // Full iteration on matrix' non-zero elements.
                                    for(const auto &[key, value]: matrix.elements)
                                        product[key[1]] += row[key[0]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            }
                        }

                    }

                    if constexpr (O == Column) {

                        if(this->compressed) {
                            if(matrix.compressed) {

                                // Iteration through matrix' columns.
                                for(std::size_t j = 0; j < matrix.columns(); ++j) { // j-th column of matrix.

                                    // Column extraction from matrix.
                                    std::vector<T> column;
                                    column.resize(matrix.rows(), static_cast<T>(0));

                                    for(std::size_t k = matrix.inner[j]; k < matrix.inner[j + 1]; ++k)
                                        column[matrix.outer[k]] = matrix.values[k];

                                    // Result's column.
                                    std::vector<T> product;
                                    product.resize(this->rows(), static_cast<T>(0));

                                    for(std::size_t k = 0; k < this->columns(); ++k) { // k-th column of this.
                                        for(std::size_t h = this->inner[k]; h < this->inner[k + 1]; ++h)
                                            product[this->outer[h]] += column[k] * this->values[h];
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            } else {

                                // Iteration through matrix' columns.
                                auto stop = (*(--matrix.elements.end())).first;
                                for(std::size_t j = 0; j < matrix.columns(); ++j) { // j-th column of matrix.

                                    // Column extraction from matrix.
                                    std::vector<T> column;
                                    column.resize(matrix.rows(), static_cast<T>(0));

                                    for(auto it = matrix.elements.lower_bound({j, 0}); (*it).first < std::array<std::size_t, 2>{j + 1, 0}; ++it) {
                                        column[(*it).first[1]] = (*it).second;

                                        if((*it).first == stop) // May cause overhead.
                                            break;
                                    }

                                    // Result's column.
                                    std::vector<T> product;
                                    product.resize(this->rows(), static_cast<T>(0));

                                    // Linear combination of this' columns.
                                    for(std::size_t k = 0; k < this->columns(); ++k) { // k-th column of this.
                                        for(std::size_t h = this->inner[k]; h < this->inner[k + 1]; ++h) {
                                            product[this->outer[h]] += column[k] * this->values[h];
                                        }
                                    }

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            }
                        } else {
                            if(matrix.compressed) {

                                // Iteration through matrix' columns.
                                for(std::size_t j = 0; j < matrix.columns(); ++j) { // j-th column of matrix.

                                    // Column extraction from matrix.
                                    std::vector<T> column;
                                    column.resize(matrix.rows(), static_cast<T>(0));

                                    for(std::size_t k = matrix.inner[j]; k < matrix.inner[j + 1]; ++k)
                                        column[matrix.outer[k]] = matrix.values[k];

                                    // Result's column.
                                    std::vector<T> product;
                                    product.resize(this->rows(), static_cast<T>(0));

                                    // Full iteration on this' non-zero elements.
                                    for(const auto &[key, value]: this->elements)
                                        product[key[0]] += column[key[1]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            } else {

                                // Iteration through matrix' columns.
                                auto stop = (*(--matrix.elements.end())).first;
                                for(std::size_t j = 0; j < matrix.columns(); ++j) { // j-th column of matrix.

                                    // Column extraction from matrix.
                                    std::vector<T> column;
                                    column.resize(matrix.rows(), static_cast<T>(0));

                                    for(auto it = matrix.elements.lower_bound({j, 0}); (*it).first < std::array<std::size_t, 2>{j + 1, 0}; ++it) {
                                        column[(*it).first[1]] = (*it).second;

                                        if((*it).first == stop) // May cause overhead.
                                            break;
                                    }

                                    // Result's column.
                                    std::vector<T> product;
                                    product.resize(this->rows(), static_cast<T>(0));

                                    // Full iteration on this' non-zero elements.
                                    for(const auto &[key, value]: this->elements)
                                        product[key[0]] += column[key[1]] * value;

                                    // Updates elements.
                                    for(std::size_t h = 0; h < product.size(); ++h) {
                                        if(std::abs(product[h]) > TOLERANCE_PACS) // Check needed.
                                            elements[{j, h}] = product[h];
                                    }
                                }

                            }
                        }

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
                template<Norm N>
                double norm() const {
                    #ifdef PARALLEL_PACS
                    auto absolute = [](const double &x) { return std::abs(x); };
                    auto absolute_squared = [](const double &x) { return std::abs(x) * std::abs(x); };
                    auto absolute_squared_map = [](auto &x) { return std::abs(x.second) * std::abs(x.second); };
                    #endif

                    double norm = 0.0, max = 0.0;

                    // On the secondary direction.
                    if constexpr (N == One) {

                        std::vector<double> sums;
                        sums.resize(this->second, 0.0);

                        if(!(this->compressed)) {

                            for(const auto &[key, value]: this->elements)
                                sums[key[1]] += std::abs(value);

                        } else {
                            
                            for(std::size_t j = 0; j < this->values.size(); ++j)
                                sums[this->outer[j]] += std::abs(this->values[j]);

                        }

                        norm = std::ranges::max(sums);

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

                            #ifdef PARALLEL_PACS
                            norm = std::sqrt(std::transform_reduce(std::execution::par, this->elements.begin(), this->elements.end(), 0.0, std::plus{}, absolute_squared_map));
                            #else
                            for(const auto &[key, value]: this->elements)
                                norm += static_cast<double>(std::abs(value) * std::abs(value));

                            norm = std::sqrt(norm);
                            #endif

                        } else {

                            #ifdef PARALLEL_PACS
                            norm = std::sqrt(std::transform_reduce(std::execution::par, this->values.begin(), this->values.end(), 0.0, std::plus{}, absolute_squared));
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
                 * @brief Returns the number of non zero elements.
                 * 
                 * @return std::size_t 
                 */
                inline std::size_t size() const {
                    if(!(this->compressed))
                        return this->elements.size();

                    return this->values.size();
                }

                /**
                 * @brief Returns the 'sparsity' of the Matrix.
                 *
                 * @return double
                 */
                inline double sparsity() const {
                    return static_cast<double>(this->size()) / static_cast<double>(this->first * this->second);
                }

                /**
                 * @brief Returns the 'density' of the Matrix.
                 *
                 * @return double
                 */
                inline double density() const {
                    return 1.0 - this->sparsity();
                }

                /**
                 * @brief Returns the order of the Matrix.
                 * 
                 * @return constexpr Order 
                 */
                constexpr Order order() const {
                    return O;
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

                // TRIVIAL GETTERS (market dumping).

                /**
                 * @brief Get the elements map.
                 * 
                 * @return std::map<std::array<std::size_t, 2>, T> 
                 */
                std::map<std::array<std::size_t, 2>, T> get_elements() const {
                    #ifndef NDEBUG
                    assert(!(this->compressed));
                    #endif

                    return this->elements;
                }

                /**
                 * @brief Get the inner vector.
                 * 
                 * @return std::vector<std::size_t> 
                 */
                std::vector<std::size_t> get_inner() const {
                    #ifndef NDEBUG
                    assert(this->compressed);
                    #endif

                    return this->inner;
                }
                
                /**
                 * @brief Get the outer vector.
                 * 
                 * @return std::vector<std::size_t> 
                 */
                std::vector<std::size_t> get_outer() const {
                    #ifndef NDEBUG
                    assert(this->compressed);
                    #endif

                    return this->outer;
                }

                /**
                 * @brief Get the values vector.
                 * 
                 * @return std::vector<T> 
                 */
                std::vector<T> get_values() const {
                    #ifndef NDEBUG
                    assert(this->compressed);
                    #endif
                    
                    return this->values;
                }
        };

    }

}

#endif