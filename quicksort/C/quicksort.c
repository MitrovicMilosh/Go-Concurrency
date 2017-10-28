#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define swap(a,b) {int t;t=a;a=b;b=t;}
#define TEST_SIZE 10000000

void error_fatal(char *message)
{
  perror(message);
  exit (EXIT_FAILURE);
}

void pthread_error(char *message, int err)
{
  fprintf(stderr, "%s : %s\n", message, strerror(err));
  exit (EXIT_FAILURE);
}

typedef struct
{
    int low;
    int hi;
    int p;
    int *a;
} qs_param;

void *QuickSortConcurrent(void *arg);
int partition( int[], int, int);

int main(int argc, char* argv[]) 
{
    int a[TEST_SIZE];
	int depth = 0;
	int i;
			
	if(argc == 2)
		depth = atoi(argv[1]);

    srand(time(NULL));

	for(i = 0; i < TEST_SIZE; i++)
		  a[i] = (rand() % TEST_SIZE);

	qs_param *qp;

	qp = (qs_param*)malloc(sizeof(qs_param));
	if(qp == NULL)
		error_fatal ("malloc error");

	qp->low = 0;
	qp->hi = TEST_SIZE-1;
	qp->p = depth;
	qp->a = a; 
	
    QuickSortConcurrent(qp);

    return 0;
}

void QuickSortSequential(int *a, int low, int hi){
	if( low < hi ) 
    {
        int j = partition(a, low, hi);
		QuickSortSequential(a, low, j - 1);
        QuickSortSequential(a, j + 1, hi);
	}
}

void *QuickSortConcurrent(void *arg)
{
    qs_param *qp;
    qp = (qs_param *) arg;
    int *a = qp->a;
    int low = qp->low;
    int hi = qp->hi;
    int p = qp->p;

    if( low < hi ) 
    {
        int j = partition(a, low, hi);

        qs_param *qp_low;
		qp_low = (qs_param*)malloc(sizeof(qs_param));
		if(qp_low == NULL)
			error_fatal ("malloc error");

        qp_low->a = a;
        qp_low->low = low;
        qp_low->hi = j - 1;
        qp_low->p = p-1; 


        qs_param *qp_hi;
		qp_hi = (qs_param*)malloc(sizeof(qs_param));
		if(qp_hi == NULL)
			error_fatal ("malloc error");

        qp_hi->a = a;
        qp_hi->low = j + 1;
        qp_hi->hi = hi;
        qp_hi->p = p-1 ;

        if(p > 0)
        {
			pthread_t* thread = malloc(2 * sizeof(pthread_t));
			if(thread == NULL)
				error_fatal ("malloc error");

			int err;
			if( (err = pthread_create(&thread[0], NULL, QuickSortConcurrent, (void*)qp_low)) != 0)
				pthread_error ("thread create error", err);
			if( (err = pthread_create(&thread[1], NULL, QuickSortConcurrent, (void*)qp_hi)) != 0)
				pthread_error ("thread create error", err);
			
			if( (err = pthread_join(thread[0], NULL)) != 0)
				pthread_error ("thread join error", err);

			if( (err = pthread_join(thread[1], NULL)) != 0)
				pthread_error ("thread join error", err);

		}
        else
        {
            QuickSortSequential(a, low, j - 1);
        	QuickSortSequential(a, j + 1, hi);
        }
    }
	return ((void*)0);
}

int partition(int a[], int low, int hi)
{
    int pivot, i, j;
    pivot = a[hi];
    i = low - 1;

    for(j=low; j<hi; j++)
		if(a[j] <= pivot){
			i++;
			swap(a[i],a[j]);
		}
			

	swap(a[hi],a[i+1]);

    return i+1;
}
