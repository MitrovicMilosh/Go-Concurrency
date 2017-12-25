//source: https://github.com/pHag/id1217/blob/master/hw2/ex2/quickSort-openmp.c

#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void quickSort(int *inputArray, int size, int depth);
void swap(int *inputArray, int leftIndex, int rightIndex);

double start_time, end_time; /* start and end times */
long size; /* array size */
/* ---------------------------------------------------------------------------- */
/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
	int i, depth, numWorkers;

	size = atoi(argv[1]);

	depth = (argc > 2)? atoi(argv[2]) : 0;
	numWorkers =(depth > 0)? pow(2,depth)-1 : 1;
	omp_set_num_threads(numWorkers); /* Set number of threads */

	int *inputArray; /* testDataArray */
	inputArray = (int*)malloc(sizeof(int) * size); /* Allocate in memory instead */
	srand(time(NULL));
	/* Create testData array */
	for (i = 0; i < size; i++) {
		inputArray[i] = rand()%99999;
	}

	struct timeval t1, t2;
	double elapsedTime;
    gettimeofday(&t1, NULL);
	/* Call the quickSort function to sort the list */
	#pragma omp parallel
	{
		/* We only want our master thread to be executed once, thus we use the singel construct here.
			nowait is used becuse we have no need for synchronization at the end of the region */
		#pragma omp single nowait
		{
			quickSort(inputArray, size, depth);
		}
	}
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0;
	printf("%f\n",elapsedTime);

	free(inputArray);
	return 0;
}

void quickSort(int *inputArray, int size, int depth){
	int pivot, leftIndex, rightIndex;
	/* End of reccursion */
	if (size <= 1) { return; }
	/* Set pivot */
	pivot = inputArray[size/2];
	for(leftIndex = 0, rightIndex = size -1;; leftIndex++, rightIndex--) {
		while(inputArray[leftIndex] < pivot){
			leftIndex++;
		}
		while(pivot < inputArray[rightIndex]){
			rightIndex--;
		}
		if(rightIndex <= leftIndex){
			break;
		}
		swap(inputArray, leftIndex, rightIndex);    
	}
	if(depth == 0){
		quickSort(inputArray, leftIndex, 0);
		quickSort(inputArray + rightIndex + 1, size - rightIndex -1, 0);
		return;
	}
	
	#pragma omp task
	{
		quickSort(inputArray, leftIndex, depth-1); /* Sort lower */
	}
	#pragma omp task
	{
		quickSort(inputArray + rightIndex + 1, size - rightIndex -1, depth-1); /* Sort upper */
	}
}
/* ---------------------------------------------------------------------------- */
/* Swaps two elements */
void swap(int *inputArray, int leftIndex, int rightIndex){
	int temp;
	temp = inputArray[leftIndex];
	inputArray[leftIndex] = inputArray[rightIndex];
	inputArray[rightIndex] = temp;
}
