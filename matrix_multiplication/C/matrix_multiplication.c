//source: http://www.cse.iitd.ernet.in/~dheerajb/Pthreads/codes/C/pthreads_MatrixMatrix.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

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
	
	return 0;
}
