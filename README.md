# Advanced Programming for Scientific Computing - Second Challenge

_A Sparse Matrix_

## Table of Contents

- [Introduction](#introduction)
- [Setup](#setup)
    - [Cloning the Repository](#cloning-the-repository)
    - [Compilation and Execution](#compilation-and-execution)

## Introduction

This repository offers an implementation of sparse matrices.

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

[^1]: This ignores some safety checks.

```make
# CXXFLAGS += -O2 -DNDEBUG
```