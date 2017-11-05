//source: https://stackoverflow.com/questions/37407048/sieve-of-eratosthenes-pthread-implementation-thread-number-doesnt-affect-compu

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
//The group of arguments passed to thread
struct thrd_data{
  long start;
  long end; /* the sub-range is from start to end */
};
//global variable
bool *GlobalList;//The list of nature number
long Num_Threads;
int n;
int limit;

void *DoSieve(void *thrd_arg)
{
  struct thrd_data *t_data;
  long start, end;

  /* Initialize my part of the global array */
  t_data = (struct thrd_data *) thrd_arg;
  start = t_data->start;
  end = t_data->end;
  int i,j;

  for (i = 2; i*i <= end && i<=limit; i++)
  {
	if (GlobalList[i] == true)
		continue;
    // skip numbers before current slice
    int minJ = ((start+i-1)/i)*i;
    if (minJ < i*i)
      minJ = i*i;	

    for (j = minJ; j <= end && GlobalList[i] != true; j += i)
    {
      GlobalList[j] = true;
    }
  }
  
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  long i, n_threads;
  long k, nq, nr;
  struct thrd_data *t_arg;
  pthread_t *thread_id;
  n = atoi(argv[1]);
  limit = (int)sqrt(n);

  if(argc>2)
	n_threads = atoi(argv[2]);
  else
    n_threads = 1;
  
  GlobalList=(bool *)malloc(sizeof(bool)*n);
  for(i=0;i<n;i++)
    GlobalList[i]=0;
  /* create arrays of thread ids and thread args */
  thread_id = (pthread_t *)malloc(sizeof(pthread_t)*n_threads);
  t_arg = (struct thrd_data *)malloc(sizeof(struct thrd_data)*n_threads);

  /* distribute load and create threads for computation */
  nq = n / n_threads;
  nr = n % n_threads;

  k = 1;
  Num_Threads=n_threads;
  for (i=0; i<n_threads; i++){
    t_arg[i].start = k;
    if (i < nr)
        k = k + nq + 1;
    else
        k = k + nq;
    t_arg[i].end = k-1;
    pthread_create(&thread_id[i], NULL, DoSieve, (void *) &t_arg[i]);
  }

  /* Wait for all threads to complete then print all prime numbers */
  for (i=0; i<n_threads; i++) {
    pthread_join(thread_id[i], NULL);
  }

  free(GlobalList);
  //pthread_attr_destroy(&attr);
  pthread_exit (NULL);
}
