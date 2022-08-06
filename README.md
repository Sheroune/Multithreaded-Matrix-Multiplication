# Multithreaded Matrix Multiplication

It is a simple project created to understand how to use multithreading by example of matrix multiplication in C++

## Compilation and run

Use makefile 
```bash
make
make run
```
Or manually
```bash
g++ -pthread Matrix.hpp main.cpp
```

## Usage

Default settings: multiplication of 2 random matrices of dimension 1000 by 1000 using 2 threads  
Change everything you need in the main file. For example:

```C++

// setting number of threads where T - matrix type, k - number of threads
Matrix<T>::SetParallel(k);

// creating a matrix of a double type with n rows and m columns
Matrix<double> a(n, m);
```
