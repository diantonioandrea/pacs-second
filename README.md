# Advanced Programming for Scientific Computing - Second Challenge

_A Sparse Matrix_

## Table of Contents

- [Introduction](#introduction)
- [Overview](#overview)
- [Setup](#setup)
    - [Cloning the Repository](#cloning-the-repository)
    - [Compilation and Execution](#compilation-and-execution)
- [Notes to the Reader](#notes-to-the-reader)
    - [On the `tester` Function](#on-the-tester-function)

## Introduction

This repository presents a header-only template implementation of sparse matrices.

It introduces the following class:

``` cpp
namespace pacs {
    namespace algebra {
        template<MatrixType T, Order O>
        class Matrix {...};
    }
}
```

which is designed to handle sparse matrices within $\mathbb{R}^{n \times m}$.

Sparse matrices can be instantiated with any type `T` that complies with the properties required by matrix spaces. Users can specify an ordering, either row-column or column-row, thanks to the following:

``` cpp
namespace pacs {
    namespace algebra {
        enum class Order {Row, Column};
    }
}
```

Storage is designed to accommodate either a dynamic allocation following a **COOmap** (Coordinate Map) format or compression into a **CSR** (Compressed Sparse Row) or **CSC** (Compressed Sparse Column) format, contingent upon its ordering. Constructors adhere to this principle by accepting either a single map for the **COOmap** format or three vectors for the **CSR**/**CSC** format, along with matrix sizing consistent with its ordering.

Reading and writing are facilitated through the following call operators:

``` cpp
T operator ()(const std::size_t &, const std::size_t &) const;
T &operator ()(const std::size_t &, const std::size_t &);
```

with the latter only acting on uncompressed matrices.

These matrices support `Matrix<T, O> * std::vector<T>` vector product and `Matrix<T, O> * Matrix<T, O>` matrix product.

Moreover, these matrices have a template method `norm` which accepts, as a template parameter, one of the followings:

``` cpp
namespace pacs {
    namespace algebra {
        enum class Norm {One, Infinity, Frobenius};
    }
}
```

and returns **the corresponding matrix norm.**

A template function `market` is also present, which enables the user to **dump and load a matrix to and from a text file** using the [Matrix Market Format](https://math.nist.gov/MatrixMarket/).

``` cpp
namespace pacs {
    namespace algebra {
        template<MatrixType T, Order O>
        Matrix<T, O> market(const std::string &, const bool &);

        template<MatrixType T, Order O>
        void market(const Matrix<T, O> &, const std::string &, const bool &);
    }
}
```

It accepts the file path and an optional verbosity flag. The dumping method also accepts the matrix.

## Overview

Key components include:

- `main.cpp`: Core script serving as a testing suite.
- `main.hpp`: Primary includes for `main.cpp`.
- `include/`:
    - `Type.hpp`: Definition for the custom Matrix' type'.
    - `Matrix.hpp`: Definition for the Matrix class.
    - `Market.hpp`: Definition for the market loader function.
    - `Tester.hpp`: Definitions for the tester functions.
- `data/`:
    - `matrix.mtx`: An example test Matrix.

## Setup

### Cloning the Repository

To begin, clone the repository from [here](https://github.com/diantonioandrea/pacs-second):

    git clone git@github.com:diantonioandrea/pacs-second.git

### Compilation and Execution

Compile the code:

    make

Execute the code:

    ./main

:warning: It is advised to redirect the output to a file due to its length by executing the code with:

    make run

Furthermore, the executable could potentially be accelerated by removing the comment's `#` from the following line in the same [Makefile](./Makefile)[^1]:

[^1]: This ignores some safety and integrity checks.

``` make
# CXXFLAGS += -DNDEBUG
```

and parallel computing can be enabled by removing, as needed, the comments' `#`s from the following lines as well[^2]:

``` make
# CXXFLAGS += -DPARALLEL_PACS
# LDFLAGS += -L$(mkTbbLib)
# LDLIBS += -ltbb
```

[^2]: Ensure the `tbb` module is loaded if needed.

## Notes to the Reader

### On the `tester` Function

Tests are designed by overloading the `tester` function multiple times in the following ways:

``` cpp
namespace pacs {
    namespace algebra {
        // Tests Matrix x Vector.
        template<typename T, Order O>
        void tester(const Matrix<T, O> &, const std::vector<T> &, const std::size_t &);

        // Tests Matrix x Matrix.
        template<typename T, Order O>
        void tester(const Matrix<T, O> &, const Matrix<T, O> &, const std::size_t &);

        // Tests Matrix x Scalar.
        template<typename T, Order O>
        void tester(const Matrix<T, O> &, const T &, const std::size_t &);

        // Tests Matrix norm.
        template<typename T, Order O>
        void tester(const Matrix<T, O> &, const std::size_t &);
    }
}
```

A single `tester` function could have been designed in the following way:

``` cpp
namespace pacs {
    namespace algebra {
        void tester(TestFunction &, const std::size_t &);
    }
}
```

This approach accepts a custom lambda function each time and times it. However, I prefer the first solution as I prefer having more customizable output and don't mind the extra code, especially since it's just for testing purposes.