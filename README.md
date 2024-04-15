# Advanced Programming for Scientific Computing - Second Challenge

_A Sparse Matrix_

## Table of Contents

- [Introduction](#introduction)
- [Setup](#setup)
    - [Cloning the Repository](#cloning-the-repository)
    - [Compilation and Execution](#compilation-and-execution)

## Introduction

This repository presents an implementation of sparse matrices at the MSc level.

It introduces the following class:

```cpp
pacs::algebra::Matrix<T, O>
```

which is designed to handle sparse matrices within $\mathbb{R}^{n \times m}$.

Sparse matrices can be instantiated with any type T, and users can specify an ordering, either row-column or column-row, thanks to the following:

```cpp
namespace pacs {
    namespace algebra {
        enum class Order { Row, Column };
    }
}
```

Furthermore, these matrices support `Matrix<T, O> * std::vector<T>` vector product and `Matrix<T, O> * Matrix<T, O>` matrix row-column product.

A template method `market` is also present, which enables the user to load a matrix from a text file using the Matrix Market Format.

```cpp
namespace pacs {
    namespace algebra {
        template<typename T, Order O>
        Matrix<T, O> market(const std::string &, const bool &);
    }
}
```

This method accepts the file path and an optional verbosity flag.

## Setup

### Cloning the Repository

To begin, clone the repository from [here](https://github.com/diantonioandrea/pacs-second):

    git clone git@github.com:diantonioandrea/pacs-second.git

### Compilation and Execution

Compile the code:

    make

Execute the code:

    ./main

Furthermore, the executable could potentially be accelerated by removing the comment from the following line in the [Makefile](./Makefile)[^1]:

[^1]: This ignores some safety and integrity checks.

``` make
# CXXFLAGS += -DNDEBUG
```