#include <stdio.h>
#include <stdlib.h>

int main (void) {
    unsigned int number = 118u;
    size_t len = 3;
    char *str = (char *)malloc(len + 30);
    snprintf(str, len + 30, "/sys/class/gpio/gpio%d/direction", number);
    fprintf(stdout, "%s\n", str);

    return 0;
}
