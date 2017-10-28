//source: https://github.com/pHag/id1217/blob/master/hw2/ex2/quickSort-openmp.c

#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define MAXSIZE 1000000 /* maximum array size */
#define MAXWORKERS 100 /* maximum number of workers */

void quickSort(int *inputArray, int size);
void swap(int *inputArray, int leftIndex, int rightIndex);

double start_time, end_time; /* start and end times */
long size; /* array size */
/* ---------------------------------------------------------------------------- */
/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
	int i, numWorkers;
	
	/* read command line args if any */
	size = (argc > 1)? atoi(argv[1]) : MAXSIZE;

	numWorkers = (argc > 2)? atoi(argv[2]) : 1;
	if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
	omp_set_num_threads(numWorkers); /* Set number of threads */

	int *inputArray; /* testDataArray */
	inputArray = (int*)malloc(sizeof(int) * size); /* Allocate in memory instead */
	srand(time(NULL));
	/* Create testData array */
	for (i = 0; i < size; i++) {
		inputArray[i] = rand()%99999;
	}

	/* Call the quickSort function to sort the list */
	#pragma omp parallel
	{
		/* We only want our master thread to be executed once, thus we use the singel construct here.
			nowait is used becuse we have no need for synchronization at the end of the region */
		#pragma omp single nowait
		{
			quickSort(inputArray, size);
		}
	}

	free(inputArray);
	return 0;
}

void quickSort(int *inputArray, int size){
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
	#pragma omp task
	{
		quickSort(inputArray, leftIndex); /* Sort lower */
	}
	#pragma omp task
	{
		quickSort(inputArray + rightIndex + 1, size - rightIndex -1); /* Sort upper */
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
