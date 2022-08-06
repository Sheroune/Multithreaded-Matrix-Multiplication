#include <iostream>
#include <fstream>

#include <thread>
#include <mutex>
#include <future>

#include <stdexcept>


template<typename T>
class Matrix{
    private:
        T** matrix;
        size_t n, m, cursor;
        
		static size_t parallel;
        std::mutex mult_mutex;
		
    public:
        Matrix() : n(0), m(0), cursor(0) {
            matrix = nullptr;
        }
		
        Matrix(size_t rows, size_t cols) : n(rows), m(cols), cursor(0) {
            matrix = new T*[n];
            for(size_t i=0; i<n; i++)
                matrix[i] = new T[m];

            for(size_t i=0; i<n; i++){
                for(size_t j=0; j<m; j++)
                    matrix[i][j] = 0;
            }
        }
		
        Matrix(const Matrix<T> &smth){
            n = smth.n;
            m = smth.m;
            matrix = smth.matrix;
        }
		
        ~Matrix() {
            for(size_t i=0;i<n;i++)
                delete[] matrix[i];
            delete[] matrix;
        }

        const size_t GetRows() const {return n;}
        const size_t GetCols() const {return m;}

        Matrix<T> operator*(const Matrix<T> &smth) const;
        
		void Multiplication(const Matrix<T> *a, const Matrix<T> *b);
        void Multithread(const Matrix<T> *a, const Matrix<T> *b, size_t thread_number, std::promise<T> &prom);
        
		static void SetParallel(size_t num) {parallel=num;}
        static size_t GetParallel() {return parallel;}
        
		void LoadMatrix(const char *filename);
        void LoadMatrix(std::ifstream &smth);
        void SaveMatrix(const char *filename) const;

        void PrintMatrix(){
            for(size_t i=0;i<n;i++){
                for(size_t j=0;j<m;j++)
                    std::cout << matrix[i][j] << " ";
                std::cout << std::endl;
            }
        }

        void RandomMatrix(){
            for(size_t i=0;i<n;i++){
                for(size_t j=0;j<m;j++)
                    matrix[i][j] = rand() % 100;
            }
        }
};

template <typename T>
void Matrix<T>::LoadMatrix(std::ifstream &smth){
    smth >> n >> m;

    matrix = new T*[n];
    for(size_t i=0; i<n; i++)
        matrix[i] = new T[m];

    for(size_t i=0; i<n; i++){
        for(size_t j=0; j<m; j++)
            matrix[i][j] = 0;
    }
    for(size_t i=0;i<n;i++){
        for(size_t j=0;j<m;j++)
            smth >> matrix[i][j];
    }
}

template <typename T>
void Matrix<T>::LoadMatrix(const char *filename){
    std::ifstream in(filename);
    in >> n >> m;

    matrix = new T*[n];
    for(size_t i=0; i<n; i++)
        matrix[i] = new T[m];

    for(size_t i=0; i<n; i++){
        for(size_t j=0; j<m; j++)
            matrix[i][j] = 0;
    }
    for(size_t i=0;i<n;i++){
        for(size_t j=0;j<m;j++)
            in >> matrix[i][j];
    }
    in.close();
}

template <typename T>
void Matrix<T>::SaveMatrix(const char *filename) const{
    std::ofstream out(filename);
    out << n << ' ' << m << std::endl;
    for(size_t i=0;i<n;i++){
        for(size_t j=0;j<m;j++)
            out << matrix[i][j] << ' ';
        out << '\n';
    }
    out.close();
}

template <typename T>
void Matrix<T>::Multiplication(const Matrix<T> *a, const Matrix<T> *b){
    for(int i = 0; i < a->GetRows(); i++){
        for(int j = 0; j < b->GetCols(); j++){
            for(int k = 0; k < a->GetCols(); k++)
                matrix[i][j] += a->matrix[i][k] * b->matrix[k][j];
        }
    }
}

template <typename T>
void Matrix<T>::Multithread(const Matrix<T> *a, const Matrix<T> *b, size_t thread_number, std::promise<T> &prom){
    while(cursor < a->GetRows()*b->GetCols()){
        mult_mutex.lock();
        const size_t local_cursor = cursor++;
        mult_mutex.unlock();

        const size_t ost1 = int((local_cursor)/a->GetRows());
        const size_t ost2 = (local_cursor)%b->GetCols();
		
        for(size_t k = 0; k < a->GetCols(); k++)
            matrix[ost1][ost2] += a->matrix[ost1][k] * b->matrix[k][ost2];
    }
    prom.set_value(thread_number+1);
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> &smth) const{
    if(GetRows()!=smth.GetCols())
        throw std::invalid_argument("Rows of matrix A != Cols of matrix B. Can't multiply");

    Matrix<T> c(GetRows(), smth.GetCols());

    std::thread* threads = new std::thread[GetParallel()];
    std::promise<T>* prom = new std::promise<T>[GetParallel()];
    std::future<T>* fut = new std::future<T>[GetParallel()];
	
	
    for(size_t i=0;i<GetParallel();i++)
        threads[i] = std::thread(&Matrix<T>::Multithread, std::ref(c), this, &smth, i, std::ref(prom[i]));
	
    for(size_t i=0;i<GetParallel();i++)
        fut[i] = prom[i].get_future();
	
    for(size_t i=0;i<GetParallel();i++)
        std::cout << "Thread " << fut[i].get() << " finished its work" << std::endl;

    for(size_t i=0;i<GetParallel();i++)
        threads[i].join();

    delete[] threads;

    return c;
}
