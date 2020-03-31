/*
	Program test_gettimeofday_usleep.c

	Test Linux function: gettimeofday() and usleep()

	Programmed by	Byung Kook Kim,	Feb. 1, 2017.
*/

#include <stdio.h>
//#include <stdlib.h>		// rand(), srand()
#include <unistd.h>			// usleep()

#include <time.h>			// clock_gettime()
#include <sys/time.h>		// gettimeofday()


int main(void)
{
  int k;						// Loop index
  int wait_us; 					// Wait time in us

  struct timeval start_tv, stop_tv;		// For gettimeofday() in s, us
  struct timezone tz;
  double elap_time_ms;			// Elapsed time in ms

  // 2. Init variables
  wait_us = 2000000UL;		// Wait time of 2 s in us

  // 3. Loop three times
  for (k=0; k<3; ++k) {
	// Get Start time
	gettimeofday(&start_tv, &tz);	// Elapsed time in us

	// Wait 2 s using usleep()
	usleep(wait_us);

	// Get Stop time
	gettimeofday(&stop_tv, &tz);

 	// Compute response time in ms
	elap_time_ms = (stop_tv.tv_sec - start_tv.tv_sec)*1e3
				 + (stop_tv.tv_usec - start_tv.tv_usec)*1e-3;

	// Print Response time  
	printf("Elapsed_time= %g ms.\n", elap_time_ms);

  } 	// End of for k

  return 0;
}

