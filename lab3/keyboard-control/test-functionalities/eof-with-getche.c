#include <stdio.h>
#include <stdlib.h>

#define esc 27
#define eof 4

char getch(void);

int main(void) {
    
    char c = getch();
    if (c == esc)
        fprintf(stdout, "escape!\n");
    else if (c == eof)
        fprintf(stdout, "eof!\n");
    else
        fprintf(stdout, "I'm fucked.\n");
    
    return 1;
}
