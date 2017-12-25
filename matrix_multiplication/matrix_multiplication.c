//source: http://www.cse.iitd.ernet.in/~dheerajb/Pthreads/codes/C/pthreads_MatrixMatrix.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

int size, n_threads, **InMat1, **InMat2, **ResMat;

pthread_t* threads;

void* doMyWork(int myRow)
{
    int i, j;
   	
	if(n_threads)
		for(; myRow < size; myRow += n_threads)
			for (j = 0; j < size; j++)
				for (i = 0; i < size; i++)
					ResMat[myRow][j] += InMat1[myRow][i] * InMat2[i][j];
	else
		for (j = 0; j < size; j++)
			for (i = 0; i < size; i++)
				ResMat[myRow][j] += InMat1[myRow][i] * InMat2[i][j];
}

int main(int argc, char* argv[])
{

    int i, j;
	bool isConcurrent = false;

    if (argc > 2){
		isConcurrent = true;
		n_threads = atoi(argv[2]);
	}

    size = atoi(argv[1]);
     
	InMat1 = (int**)malloc(sizeof(int*) * size);
    InMat2 = (int**)malloc(sizeof(int*) * size);
    ResMat = (int**)malloc(sizeof(int*) * size); 
	
	for (i = 0; i < size; i++){
        InMat1[i] = (int*)malloc(sizeof(int) * size);
		InMat2[i] = (int*)malloc(sizeof(int) * size);
		ResMat[i] = (int*)malloc(sizeof(int) * size);
	}

    for (i = 0; i < size; i++){
        for (j = 0; j < size; j++){
            InMat1[i][j] = i;
			InMat2[i][j] = j;
			ResMat[i][j] = 0;
		}     
	}
	
	struct timeval t1, t2;
    double elapsedTime;
    gettimeofday(&t1, NULL);
	if(isConcurrent){
		threads = (pthread_t*)malloc(sizeof(pthread_t) * size);
		
		for (i = 0; i < n_threads; i++) 
			pthread_create(&threads[i], NULL, (void* (*)(void*))doMyWork, (void*)i);
		
		for (i = 0; i < n_threads; i++) {
			pthread_join(threads[i], NULL);
    }
	}else{
		for (i = 0; i < size; i++)
			doMyWork(i);
	}
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0;
	printf("%f\n",elapsedTime);
	
	return 0;
}
