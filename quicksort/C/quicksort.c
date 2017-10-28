//source: http://cs.swan.ac.uk/~csdavec/HPC/sort.c.html

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

// Macro for swapping two values.
#define SWAP(x,y) do {\
    __typeof__(x) tmp = x;\
    x = y;\
    y = tmp;\
} while(0)

int partition(int *array, int left, int right, int pivot)
{
    int pivotValue = array[pivot];
    SWAP(array[pivot], array[right]);
    int storeIndex = left;
    for (int i=left ; i<right ; i++)
    {
        if (array[i] <= pivotValue)
        {
            SWAP(array[i], array[storeIndex]);
            storeIndex++;
        }
    }
    SWAP(array[storeIndex], array[right]);
    return storeIndex;
}

void dump(int *array, int size)
{
    for (int i=0 ; i<size ; i++)
    {
        printf("[%d] %d\n", i, array[i]);
    }
}

void quicksort(int *array, int left, int right)
{
     if (right > left)
     {
        int pivotIndex = left + (right - left)/2;
        pivotIndex = partition(array, left, right, pivotIndex);
        quicksort(array, left, pivotIndex-1);
        quicksort(array, pivotIndex+1, right);
     }
}

struct qsort_starter
{
    int *array;
    int left;
    int right;
    int depth;
};

void parallel_quicksort(int *array, int left, int right, int depth);

void* quicksort_thread(void *init)
{
    struct qsort_starter *start = init;
    parallel_quicksort(start->array, start->left, start->right, start->depth);
    return NULL;
}

void parallel_quicksort(int *array, int left, int right, int depth)
{
    if (right > left)
    {
        int pivotIndex = left + (right - left)/2;
        pivotIndex = partition(array, left, right, pivotIndex);

        if (depth-- > 0)
        {
            struct qsort_starter arg1 = {array, left, pivotIndex-1, depth};
            pthread_t thread1;
            pthread_create(&thread1, NULL, quicksort_thread, &arg1);
			
			struct qsort_starter arg2 = {array, pivotIndex+1, right, depth};
            pthread_t thread2;
            pthread_create(&thread2, NULL, quicksort_thread, &arg2);

            pthread_join(thread1, NULL);
			pthread_join(thread2, NULL);
        }
        else
        {
            quicksort(array, left, pivotIndex-1);
            quicksort(array, pivotIndex+1, right);
        }
    }
}

int main(int argc, char **argv)
{

    int depth = 0;
	int size = 10000000;
    if (argc > 1)
    {
		size = strtol(argv[1], NULL, 10);
    }
    if (argc > 2)
    {
        depth = strtol(argv[2], NULL, 10);
    }

    srand(42);
    int *values = malloc(size * sizeof(int));
    assert(values && "Allocation failed");
    for (int i=0 ; i<size ; i++)
    {
        values[i] = rand()%size;
    }
    parallel_quicksort(values, 0, size-1, depth);

    return 0;
}