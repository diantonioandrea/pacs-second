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

// Zero tolerance.
#ifndef TOLERANCE_PACS
#define TOLERANCE_PACS 1E-8
#endif

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
                 * @brief Construct a new empty Matrix<T, O>.
                 *
                 * @param first
                 * @param second
                 */
                Matrix<T, O>(const std::size_t &first, const std::size_t &second): first{first}, second{second} {}

                /**
                 * @brief Construct a new Matrix<T, O> from a given std::map.
                 *
                 * @param first
                 * @param second
                 * @param elements
                 */
                Matrix<T, O>(const std::size_t &first, const std::size_t &second, const std::map<std::array<std::size_t, 2>, T> elements):
                first{first}, second{second}, elements{elements} {
                    #ifndef NDEBUG // Integrity checks.
                    assert((first > 0) && (second > 0));

                    // WIP.

                    #endif
                }

                /**
                 * @brief Construct a new Matrix<T, O> from given inner, outer and values vectors.
                 *
                 * @param first
                 * @param second
                 * @param elements
                 */
                Matrix<T, O>(const std::size_t &first, const std::size_t &second, const std::vector<std::size_t> &inner, const std::vector<std::size_t> &outer, const std::vector<T> &values):
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
                Matrix<T, O>(const Matrix<T, O> &matrix): first{matrix.first}, second{matrix.second} {
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
                 * @return Matrix<T, O>&
                 */
                Matrix<T, O> &operator =(const Matrix<T, O> &matrix) {
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
                 * @return Matrix<T, O>
                 */
                Matrix<T, O> reshape(const std::size_t &first, const std::size_t &second, const bool &compress = false) const {
                    if(!(this->compressed)) {
                        Matrix<T, O> matrix{first, second, this->elements};

                        if(compress)
                            matrix.compress();

                        return matrix;
                    }

                    Matrix<T, O> matrix{first, second, this->inner, this->outer, this->values};

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

                            // std::abs : T -> floating_point must be defined for this to work.
                            if(std::abs(value) > TOLERANCE_PACS) {
                                this->outer.emplace_back(key[1]);
                                this->values.emplace_back(value);
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
                inline bool is_compressed() const {
                    return this->compressed;
                }

                // ROWS AND COLUMNS.

                /**
                 * @brief Returns the j-th row.
                 *
                 * @param j
                 * @return std::vector<T>
                 */
                std::vector<T> row(const std::size_t &j) const {
                    #ifndef NDEBUG
                    assert(j < this->rows());
                    #endif

                    std::vector<T> row;
                    row.resize(this->columns(), static_cast<T>(0));

                    if constexpr (O == Row) {
                        if(!(this->compressed)) {
                            auto end = j + 1 == this->rows() ? --this->elements.end() : this->elements.lower_bound({j + 1, 0}); // End iterator.
                            for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*end).first; ++it)
                                row[(*it).first[1]] = (*it).second;

                            if(j + 1 == this->rows()) // Last element fix.
                                row[this->columns() - 1] = (*this)(j, this->columns() - 1);

                        } else {
                            for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i)
                                row[this->outer[i]] = this->values[i];
                        }
                    }

                    if constexpr (O == Column) {
                        if(!(this->compressed)) { // Full iteration on non-zero elements.
                            for(const auto &[key, value]: this->elements) {
                                if(key[1] == j)
                                    row[key[0]] = value;
                            }
                        } else {
                            for(std::size_t i = 0; i < this->inner.size() - 1; ++i) {
                                for(std::size_t k = this->inner[i]; k < this->inner[i + 1]; ++k) {
                                    if(this->outer[k] == j) {
                                        row[i] = this->values[k];
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    return row;
                }

                /**
                 * @brief Returns the j-th column.
                 *
                 * @param j
                 * @return std::vector<T>
                 */
                std::vector<T> column(const std::size_t &j) const {
                    #ifndef NDEBUG
                    assert(j < this->columns());
                    #endif

                    std::vector<T> column;
                    column.resize(this->rows(), static_cast<T>(0));

                    if constexpr (O == Column) {
                        if(!(this->compressed)) {
                            auto end = j + 1 == this->rows() ? --this->elements.end() : this->elements.lower_bound({j + 1, 0}); // End iterator.
                            for(auto it = this->elements.lower_bound({j, 0}); (*it).first < (*end).first; ++it)
                                column[(*it).first[1]] = (*it).second;

                            if(j + 1 == this->rows()) // Last element fix.
                                column[this->columns() - 1] = (*this)(j, this->columns() - 1);

                        } else {
                            for(std::size_t i = this->inner[j]; i < this->inner[j + 1]; ++i)
                                column[this->outer[i]] = this->values[i];
                        }
                    }

                    if constexpr (O == Row) {
                        if(!(this->compressed)) { // Full iteration on non-zero elements.
                            for(const auto &[key, value]: this->elements) {
                                if(key[1] == j)
                                    column[key[0]] = value;
                            }
                        } else {
                            for(std::size_t i = 0; i < this->inner.size() - 1; ++i) {
                                for(std::size_t k = this->inner[i]; k < this->inner[i + 1]; ++k) {
                                    if(this->outer[k] == j) {
                                        column[i] = this->values[k];
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    return column;
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

                // OPERATIONS.

                /**
                 * @brief Returns the product of Matrix x (column) Vector.
                 *
                 * @param vector
                 * @return std::vector<T>
                 */
                std::vector<T> operator *(const std::vector<T> &vector) const {
                    #ifndef NDEBUG // Vector size check.
                    assert(vector.size() == this->rows());
                    #endif

                    std::vector<T> result;
                    result.resize(vector.size());

                    for(std::size_t j = 0; j < vector.size() ; ++j) {
                        std::vector<T> row = this->row(j);

                        for(std::size_t k = 0; k < this->columns(); ++k) {
                            result[j] += row[k] * vector[k];
                        }
                    }

                    return result;
                }

                /**
                 * @brief Returns the product of (row) Vector x Matrix.
                 *
                 * @param vector
                 * @param matrix
                 * @return std::vector<T>
                 */
                friend std::vector<T> operator *(const std::vector<T> &vector, const Matrix<T, O> &matrix) {
                    #ifndef NDEBUG // Vector size check.
                    assert(vector.size() == matrix.columns());
                    #endif

                    std::vector<T> result;
                    result.resize(vector.size());

                    for(std::size_t j = 0; j < vector.size() ; ++j) {
                        std::vector<T> column = matrix.column(j);

                        for(std::size_t k = 0; k < matrix.rows(); ++k) {
                            result[j] += vector[k] * column[k];
                        }
                    }

                    return result;
                }

                /**
                 * @brief Returns the Matrix x Matrix product.
                 *
                 * @param matrix
                 * @return Matrix<T, O>
                 */
                Matrix<T, O> operator *(const Matrix<T, O> &matrix) const {
                    #ifndef NDEBUG
                    assert(this->columns() == matrix.rows());
                    #endif

                    std::map<std::array<std::size_t, 2>, T> elements;

                    for(std::size_t j = 0; j < this->rows(); ++j) {
                        for(std::size_t k = 0; k < matrix.columns(); ++k) {
                            std::vector<T> row = this->row(j);
                            std::vector<T> column = matrix.column(k);
                            T product = static_cast<T>(0);

                            for(std::size_t h = 0; h < row.size(); ++h)
                                product += row[h] * column[h];

                            if constexpr (O == Row) {
                                elements[{j, k}] = product;
                            } else {
                                elements[{k, j}] = product;
                            }
                        }
                    }

                    std::size_t first = O == Row ? this->rows() : matrix.columns();
                    std::size_t second = O == Row ? matrix.columns() : this->rows();

                    Matrix<T, O> result{first, second, elements};

                    if((this->compressed) || (matrix.compressed))
                        result.compress();

                    return result;
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
                    double norm = 0.0, max = 0.0;

                    if constexpr (N == One) {

                        for(std::size_t j = 0; j < this->columns(); ++j) {
                            double sum = 0.0;
                            std::vector<T> column = this->column(j);

                            for(const auto &value: column) // Inefficient.
                                sum += std::abs(value);

                            max = sum > max ? sum : max;
                        }

                        norm = max;

                    }

                    if constexpr (N == Infinity) {

                        for(std::size_t j = 0; j < this->rows(); ++j) {
                            double sum = 0.0;
                            std::vector<T> row = this->row(j);

                            for(const auto &value: row) // Inefficient.
                                sum += std::abs(value);

                            max = sum > max ? sum : max;
                        }

                        norm = max;

                    }

                    if constexpr (N == Frobenius) {

                        if(!(this->compressed)) {
                            for(const auto &[key, value]: this->elements)
                                norm += static_cast<double>(std::abs(value) * std::abs(value));
                        } else {
                            for(const auto &value: this->values)
                                norm += static_cast<double>(std::abs(value) * std::abs(value));
                        }

                        norm = std::sqrt(norm);

                    }

                    return norm;
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