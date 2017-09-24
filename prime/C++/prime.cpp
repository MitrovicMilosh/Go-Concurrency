// //////////////////////////////////////////////////////////
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//


typedef int Number;  // or for the brave ones: typedef __int64 Number;
const Number lastNumber = 1*1*1000*1000LL;

#include <stdio.h>
#include <math.h>
#include <string.h>

// OpenMP
#include <omp.h>
// block size, 128k is the sweet spot for Core i7 cpus
const int sliceSize = 128*1024;


// simple serial sieve of Eratosthenes
int eratosthenes(Number lastNumber)
{
  // initialize
  char* isPrime = new char[lastNumber+1];
  for (Number i = 0; i <= lastNumber; i++)
    isPrime[i] = 1;

  // find all non-primes
  for (Number i = 2; i*i <= lastNumber; i++)
    if (isPrime[i])
      for (Number j = i*i; j <= lastNumber; j += i)
        isPrime[j] = 0;

  // sieve is complete, count primes
  int found = 0;
  for (Number i = 2; i <= lastNumber; i++)
    found += isPrime[i];

  delete[] isPrime;
  return found;
}


// odd-only sieve
int eratosthenesOdd(Number lastNumber, bool useOpenMP)
{
  // enable/disable OpenMP
  omp_set_num_threads(useOpenMP ? omp_get_num_procs() : 1);

  // instead of i*i <= lastNumber we write i <= lastNumberSquareRoot to help OpenMP
  const Number lastNumberSqrt = (int)sqrt((double)lastNumber);

  Number memorySize = (lastNumber-1)/2;

  // initialize
  char* isPrime = new char[memorySize+1];
#pragma omp parallel for
  for (Number i = 0; i <= memorySize; i++)
    isPrime[i] = 1;

  // find all odd non-primes
#pragma omp parallel for schedule(dynamic)
  for (Number i = 3; i <= lastNumberSqrt; i += 2)
    if (isPrime[i/2])
      for (Number j = i*i; j <= lastNumber; j += 2*i)
        isPrime[j/2] = 0;

  // sieve is complete, count primes
  int found = lastNumber >= 2 ? 1 : 0;
#pragma omp parallel for reduction(+:found)
  for (Number i = 1; i <= memorySize; i++)
    found += isPrime[i];

  delete[] isPrime;
  return found;
}


// process only odd numbers of a specified block
int eratosthenesOddSingleBlock(const Number from, const Number to)
{
  const Number memorySize = (to - from + 1) / 2;

  // initialize
  char* isPrime = new char[memorySize];
  for (Number i = 0; i < memorySize; i++)
    isPrime[i] = 1;

  for (Number i = 3; i*i <= to; i += 2)
  {
    // skip multiples of three: 9, 15, 21, 27, ...
    if (i >= 3*3 && i % 3 == 0)
      continue;
    // skip multiples of five
    if (i >= 5*5 && i % 5 == 0)
      continue;
    // skip multiples of seven
    if (i >= 7*7 && i % 7 == 0)
      continue;
    // skip multiples of eleven
    if (i >= 11*11 && i % 11 == 0)
      continue;
    // skip multiples of thirteen
    if (i >= 13*13 && i % 13 == 0)
      continue;

    // skip numbers before current slice
    Number minJ = ((from+i-1)/i)*i;
    if (minJ < i*i)
      minJ = i*i;

    // start value must be odd
    if ((minJ & 1) == 0)
      minJ += i;

    // find all odd non-primes
    for (Number j = minJ; j <= to; j += 2*i)
    {
      Number index = j - from;
      isPrime[index/2] = 0;
    }
  }

  // count primes in this block
  int found = 0;
  for (Number i = 0; i < memorySize; i++)
    found += isPrime[i];
  // 2 is not odd => include on demand
  if (from <= 2)
    found++;

  delete[] isPrime;
  return found;
}


// process slice-by-slice, odd numbers only
int eratosthenesBlockwise(Number lastNumber, Number sliceSize, bool useOpenMP)
{
  // enable/disable OpenMP
  omp_set_num_threads(useOpenMP ? omp_get_num_procs() : 1);

  int found = 0;
  // each slices covers ["from" ... "to"], incl. "from" and "to"
#pragma omp parallel for reduction(+:found)
  for (Number from = 2; from <= lastNumber; from += sliceSize)
  {
    Number to = from + sliceSize;
    if (to > lastNumber)
      to = lastNumber;

    found += eratosthenesOddSingleBlock(from, to);
  }

  return found;
}


int main(Number argc, char* argv[])
{
  if(!strcmp(argv[1],"1"))
  	eratosthenes(lastNumber);
  else if(!strcmp(argv[1],"2"))
  	eratosthenesOdd(lastNumber, false);
  else if(!strcmp(argv[1],"3"))
  	eratosthenesOdd(lastNumber, true);
  else if(!strcmp(argv[1],"4"))
  	eratosthenesBlockwise(lastNumber, 2*sliceSize, false);
  else if(!strcmp(argv[1],"5"))
  	eratosthenesBlockwise(lastNumber, 2*sliceSize, true);
  
  return 0;
}
