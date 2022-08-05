FLAGS = -pthread -g0 -DNDEBUG -s -march=native -fno-stack-protector
 

all: main.o
 
main.o:	Matrix.hpp main.cpp
	g++ $(FLAGS) Matrix.hpp main.cpp

run:
	./a.out
	
clean:
	rm ./a.out
	clear