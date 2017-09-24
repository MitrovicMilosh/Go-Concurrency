#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int size, currentRow = 0, **InMat1, **InMat2, **ResMat;

pthread_t* threads;
int numberOfThreads;

pthread_mutex_t mutex_Row = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t count_threshold_cv = PTHREAD_COND_INITIALIZER;

void* doMyWork(int Id)
{

    int i, j, myRow, cnt;
    while (1) {
 
        pthread_mutex_lock(&mutex_Row);

        if (currentRow >= size) {
            pthread_mutex_unlock(&mutex_Row);

            if (Id == 0)
                return ((void*)0);

            pthread_exit(0);
        }
        myRow = currentRow;
        currentRow++;
        pthread_mutex_unlock(&mutex_Row);

        for (j = 0; j < size; j++)
            for (i = 0; i < size; i++)
                ResMat[myRow][j] += InMat1[myRow][i] * InMat2[i][j];
    }
}

int main(int argc, char* argv[])
{

    int i, j;

    if (argc < 3) {
        printf("\n Insufficient argumets. \n Usage:");
        printf(" exe size threads.\n");
        return 0;
    }

    size = abs(atoi(argv[1]));
    
   	numberOfThreads = abs(atoi(argv[2]));
 
	InMat1 = (int**)malloc(sizeof(int) * size);
    for (i = 0; i < size; i++)
        InMat1[i] = (int*)malloc(sizeof(int) * size);

    InMat2 = (int**)malloc(sizeof(int) * size);
    for (i = 0; i < size; i++)
        InMat2[i] = (int*)malloc(sizeof(int) * size);

    ResMat = (int**)malloc(sizeof(int) * size);
    for (i = 0; i < size; i++)
        ResMat[i] = (int*)malloc(sizeof(int) * size);


    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            InMat1[i][j] = i;

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            InMat2[i][j] = j;

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
            ResMat[i][j] = 0;

    threads = (pthread_t*)malloc(sizeof(pthread_t) * numberOfThreads);

    currentRow = 0;

    for (i = 0; i < numberOfThreads; i++) {
        pthread_create(&threads[i], NULL, (void* (*)(void*))doMyWork, (void*)(i + 1));
    }

    for (i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

	return 0;
}
