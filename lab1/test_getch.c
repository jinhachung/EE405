/* *******************************************************************
 * Problem 1C - test_getch.c for Reaction Timer component testing
 * a. print information,
 *    "Test getch(): Please type 16 letters sequentially.\n"
 * b. loop 16 times:
 *      > b-1: get a key without Enter using getch()
 *      > b-2: print the key three times with formats "%c, %d, %2xh; ",
        > with printf("%c, %d, %02xh; ", c, c, c);
 * c. print "\n"
 * COMPILE WITH MULTIPLE FILES: test_getch.c and getche.c,
 *                              since getche.c will be used later
 ****************************************************************** */
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// inform compiler about arguments and return values: conform to actual function in getche.c
char getch(void);
char getche(void);

int main(void)
{
    // user-input character
    char c;
    // print information
    fprintf(stdout, "Test getch(): Please type 16 letters sequentially.\n");
    // repeat 16 times
    for (int i = 0; i < 16; ++i) {
        c = getch();
        fprintf(stdout, "%c, %d, %02xh; ", c, c, c);
        fflush(NULL);
    }
    fprintf(stdout, "\n");
    return 0;
}


