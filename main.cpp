#include <chrono>

#include "Matrix.hpp"

using namespace std;

template<typename T>
size_t Matrix<T>::parallel = 0;

int main(){
    Matrix<double>::SetParallel(2);

    Matrix<double> a(1000, 1000);
    Matrix<double> b(1000, 1000);
    
    a.RandomMatrix();
    b.RandomMatrix();
	
    auto start = chrono::system_clock::now();
    Matrix<double> c = a*b;
    auto end = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::seconds>(end - start);
    cout << "Elapsed time: " << elapsed.count() << "s" << endl;

    c.SaveMatrix("out.txt");
    return 0;
}
