/*
	Program test_rand.c  

	Test function rand()

	Programmed by	Byung Kook Kim,	Feb. 1, 2017
*/

#include <stdio.h>
#include <stdlib.h>		// rand(), srand()
#include <time.h>		// time()


int main(void)
{
  int k;			// Loop index
  double Lower, Upper, Range;	// Random number range
  double rn[100];	// Random number array

  // 1. Init variables
  Lower = 1.;
  Upper = 10.;
  Range = Upper - Lower;
  srand(time(NULL));		// Seed for rand

  // 2. Loop 100 times
  for (k=0; k<100; ++k) {

	// Generate a random number [Lower, Upper] to rn[k]
	rn[k] = Lower + Range*rand()/RAND_MAX;
  }

  // 3. Compute avg, min, and max of rm[k]
  double Min = 1e9;
  double Max = -1e9;
  double sum = 0.;
  for (k=0; k<100; ++k) {
	sum += rn[k]; 
	if (rn[k] < Min) Min = rn[k];
	if (rn[k] > Max) Max = rn[k];
  }
  double Avg = sum/100.;

  // 4. Print result
  printf("Rand(1 to 10) Avg= %g, Min= %g, Max= %g\n", Avg, Min, Max);

  return 0;
}

