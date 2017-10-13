#!/bin/bash

SUF=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 10 | head -n 1)
FILENAME="results_$SUF.txt"

M_SIZE=700
M_THREADS=20

Q_SIZE=1000000
Q_THREADS=20
Q_DEPTH=5

P_SIZE=100000000
P_THREADS=20
P_GOROUTINES=1000
P_BLOCK_SIZE=32000

echo "matrix concurrent, size: $M_SIZE, thread num: $M_THREADS" 2>&1 | tee $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./matrix_go $M_SIZE $M_THREADS  2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./matrix_c $M_SIZE $M_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./matrix_omp $M_SIZE $M_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'std: %E s %M kB' ./matrix_std $M_SIZE $M_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./matrix/matrix.py 200 $M_THREADS 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME

echo "matrix sequential" 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./matrix_go $M_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./matrix_c $M_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./matrix_omp $M_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'std: %E s %M kB' ./matrix_std $M_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./matrix/matrix.py 200 1 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME

echo "quicksort concurrent, size: $Q_SIZE, thread num/depth: $Q_THREADS/$Q_DEPTH" 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./quicksort_go $Q_SIZE $Q_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./quicksort_c $Q_SIZE $Q_DEPTH 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./quicksort_omp $Q_SIZE $Q_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'std: %E s %M kB' ./quicksort_std $Q_SIZE $Q_DEPTH 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./quicksort/quicksort.py 100000 $Q_THREADS 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME

echo "quicksort sequential" 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./quicksort_go $Q_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./quicksort_c $Q_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./quicksort_omp $Q_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'std: %E s %M kB' ./quicksort_std $Q_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./quicksort/quicksort.py 100000 1 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME

echo "prime concurrent, size: $P_SIZE, thread num/goroutines/block_size: $P_THREADS/$P_GOROUTINES/$P_BLOCK_SIZE" 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./prime_go $P_SIZE $P_GOROUTINES 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./prime_c $P_SIZE $P_THREADS 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./prime_cpp $P_SIZE $P_BLOCK_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./prime/prime.py 1000000 $P_THREADS 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME

echo "prime sequential" 2>&1 | tee -a $FILENAME
echo "" 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'go: %E s %M kB' ./prime_go $P_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'c: %E s %M kB' ./prime_c $P_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'omp: %E s %M kB' ./prime_cpp $P_SIZE 2>&1 | tee -a $FILENAME
/usr/bin/time -f 'py: %E s %M kB' python ./prime/prime.py 1000000 1 2>&1 | tee -a $FILENAME
