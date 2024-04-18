# Advanced Programming for Scientific Computing - Second Challenge

_A Sparse Matrix_

## Table of Contents

- [Introduction](#introduction)
- [Overview](#overview)
- [Setup](#setup)
    - [Cloning the Repository](#cloning-the-repository)
    - [Compilation and Execution](#compilation-and-execution)
- [A note on `Matrix<T, O> * Matrix<T, O>`](#a-note-on-matrixt-o--matrixt-o)

## Introduction

This repository presents a header-only template implementation of sparse matrices.

It introduces the following class:

```cpp
namespace pacs {
    namespace algebra {
        template<typename T, Order O>
        class Matrix {...};
    }
}
```

which is designed to handle sparse matrices within $\mathbb{R}^{n \times m}$.

Sparse matrices can be instantiated with any type `T`, and users can specify an ordering, either row-column or column-row, thanks to the following:

```cpp
namespace pacs {
    namespace algebra {
        enum class Order { Row, Column };
    }
}
```

Storage is designed to accommodate either a dynamic allocation following a **COOmap** (Coordinate Map) format or compression into a **CSR** (Compressed Sparse Row) or **CSC** (Compressed Sparse Column) format, contingent upon its ordering. Constructors adhere to this principle by accepting either a single map for the **COOmap** format or three vectors for the **CSR**/**CSC** format, along with matrix sizing consistent with its ordering.

Reading and writing are facilitated through the following call operators:

```cpp
T operator ()(const std::size_t &, const std::size_t &) const;
T &operator ()(const std::size_t &, const std::size_t &);
```

with the latter only acting on uncompressed matrices.

These matrices support `Matrix<T, O> * std::vector<T>` vector product and `Matrix<T, O> * Matrix<T, O>` matrix row-column product.

Moreover, these matrices have a template method `norm` which accepts, as a template parameter, one of the followings:

```cpp
namespace pacs {
    namespace algebra {
        enum class Norm { One, Infinity, Frobenius };
    }
}
```

and returns the corresponding matrix norm.

A template function `market` is also present, which enables the user to load a matrix from a text file using the Matrix Market Format.

```cpp
namespace pacs {
    namespace algebra {
        template<typename T, Order O>
        Matrix<T, O> market(const std::string &, const bool &);
    }
}
```

This method accepts the file path and an optional verbosity flag.

## Overview

Key components include:

- `main.cpp`: Core script serving as a testing suite.
- `main.hpp`: Primary includes for `main.cpp`.
- `include/`:
    - `Matrix.hpp`: Definition for the Matrix class.
    - `Market.hpp`: Definition for the market loader function.
    - `Timer.hpp`: Definition for the timer function.

## Setup

### Cloning the Repository

To begin, clone the repository from [here](https://github.com/diantonioandrea/pacs-second):

    git clone git@github.com:diantonioandrea/pacs-second.git

### Compilation and Execution

Compile the code:

    make

Execute the code:

    ./main

Furthermore, the executable could potentially be accelerated by removing the comment from the following line in the same [Makefile](./Makefile)[^1]:

[^1]: This ignores some safety and integrity checks.

``` make
# CXXFLAGS += -DNDEBUG
```

and parallel computing can be enabled by removing the comment from the following lines as well[^2]:

``` make
# CXXFLAGS += -DPARALLEL_PACS
# LDFLAGS += -L$(mkTbbLib)
# LDLIBS += -ltbb
```

[^2]: :warning: Ensure the `tbb` module is loaded.

## A note on `Matrix<T, O> * Matrix<T, O>`

_To be added._