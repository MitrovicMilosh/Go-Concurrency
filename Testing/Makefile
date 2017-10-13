all:
	gcc -o quicksort_c -pthread -std=c99 ./quicksort/quicksort.c
	g++ -o quicksort_omp -fopenmp ./quicksort/quicksort_omp.cpp
	g++ -o quicksort_std -pthread ./quicksort/quicksort_std.cpp
	go build -o quicksort_go ./quicksort/quicksort.go
	gcc -o matrix_c -pthread ./matrix/matrix.c
	g++ -o matrix_omp -fopenmp ./matrix/matrix_omp.cpp
	g++ -o matrix_std -pthread ./matrix/main.cpp ./matrix/matrixnn.hpp ./matrix/thread_helpers.hpp
	go build -o matrix_go ./matrix/matrix.go
	gcc -o prime_c -pthread ./prime/prime.c -lm
	g++ -o prime_cpp -fopenmp ./prime/prime.cpp
	go build -o prime_go ./prime/prime.go
