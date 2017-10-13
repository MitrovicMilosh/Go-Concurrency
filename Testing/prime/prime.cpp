// //////////////////////////////////////////////////////////
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//


typedef int Number;  // or for the brave ones: typedef __int64 Number;
Number lastNumber = 1*500*1000*1000LL;
 
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// OpenMP
#include <omp.h>
// block size, 128k is the sweet spot for Core i7 cpus
int sliceSize = 32*1024;
char* isPrime = new char[lastNumber];
int limit = (int)sqrt(lastNumber);

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

  if(argc>2){
	sliceSize = atoi(argv[2]);
  	eratosthenesBlockwise(lastNumber, 2*sliceSize, true);
  }else{
	eratosthenesBlockwise(lastNumber, 2*sliceSize, false);
  }
  
  return 0;
}
