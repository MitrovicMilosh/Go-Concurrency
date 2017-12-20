//source: http://create.stephan-brumme.com/eratosthenes/

typedef int Number;  
Number lastNumber;
 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <chrono>

// OpenMP
#include <omp.h>
// block size, 128k is the sweet spot for Core i7 cpus
int sliceSize = 32*1024;
char* isPrime;
int limit;

void eratosthenesSingleBlock(const Number from, const Number to)
{

  for (Number i = 2; i*i <= to && i<=limit; i++)
  {
	if (isPrime[i] == 1)
		continue;
    // skip numbers before current slice
    Number minJ = ((from+i-1)/i)*i;
    if (minJ < i*i)
      minJ = i*i;

    for (Number j = minJ; j <= to && isPrime[i] != 1; j += i)
    {
      isPrime[j] = 1;
    }
  }
}

void eratosthenesBlockwise(Number lastNumber, Number sliceSize, bool useOpenMP)
{
  // enable/disable OpenMP
  omp_set_num_threads(useOpenMP ? omp_get_num_procs() : 1);

  // each slices covers ["from" ... "to"], incl. "from" and "to"
#pragma omp parallel for 
  for (Number from = 2; from <= lastNumber; from += sliceSize)
  {
    Number to = from + sliceSize;
    if (to > lastNumber)
      to = lastNumber;

	eratosthenesSingleBlock(from, to);
  }
}

int main(Number argc, char* argv[])
{
  lastNumber = atoi(argv[1]);
  isPrime = new char[lastNumber];
  limit = (int)sqrt(lastNumber);
  
  auto start = std::chrono::high_resolution_clock::now();
  if(argc>2){
	sliceSize = atoi(argv[2]);
  	eratosthenesBlockwise(lastNumber, 2*sliceSize, true);
  }else{
	eratosthenesBlockwise(lastNumber, 2*sliceSize, false);
  }
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  printf("%lf\n",elapsed.count());
  
  return 0;
}
