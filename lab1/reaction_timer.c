/* *******************************************************************
 * Problem 1C - Reaction Timer
 * repeat the following a-h 3 times:
 * a. random generate time T                -> rand()
 *    wait random time T+2                  -> usleep()
 * b. random generate char x ('f' or 'j')   -> rand()
 * c. output string "Type the 'x' character:"
 * d. get start time                        -> gettimeofday()
 * e. wait until user hits a key, input in raw mode and reply
 * f. get stop time                         -> gettimeofday()
 * g. compute response time = stop time - start time (in ms)
 * h. print key correctness and response time
 * COMPILE WITH MULTIPLE FILES: reaction_timer.c and getche.c,
 *                              since getche.c will be used later
 ****************************************************************** */
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

// inform compiler about arguments and return values: conform to actual function in getche.c
char getch(void);
char getche(void);

int main(void)
{
    // user-input character
    char c;
    // variables for computing elapsed time
    struct timeval start_tv, stop_tv;
    struct timezone tz;
    double reaction_time_in_ms;
    // set seed for rand()
    srand(time(NULL));
    // repeat 3 times
    for (int i = 0; i < 3; ++i) {
        // a: generate random time T and wait for time T+2
        double gen_time = ( (double)rand() / (double)RAND_MAX ) * 3;
        double wait_time = gen_time + 2;
        usleep(wait_time * 1e6);
        // b: generate random char x, either f or j
        char key = (rand() % 2 == 0) ? 'f' : 'j';
        // c: output string
        fprintf(stdout, "Type the '%c' character:", key);
        fflush(NULL);
        // d: get start time
        gettimeofday(&start_tv, &tz);
        // e: wait until user hits a key, input in raw mode and reply
        c = getch();
        fprintf(stdout, " %c\n", c);
        // f: get stop time
        gettimeofday(&stop_tv, &tz);
        // g: compute response time
        reaction_time_in_ms = (stop_tv.tv_sec - start_tv.tv_sec) * 1e3
                             + (stop_tv.tv_usec - start_tv.tv_usec) * 1e-3;
        // h: print key correctness and response time
        if (key == c)
            fprintf(stdout, "  Correct response %c in %g ms.\n", c, reaction_time_in_ms);
        else
            fprintf(stdout, "  Incorrect response %c in %g ms.\n", c, reaction_time_in_ms);
    }
    return 0;
}
