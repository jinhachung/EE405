#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int fd = open("mytest.txt", O_RDWR);
    char *buffer = (char *) malloc(100);
    size_t num_bytes = read(fd, buffer, 10);

    fprintf(stdout, "%s\n", buffer);
    fprintf(stdout, "read: %d bytes\n", (int)num_bytes);
    if (buffer[0] == '0')
        fprintf(stdout, "zero\n");
    else if (buffer[0] == '1')
        fprintf(stdout, "one\n");
    else
        fprintf(stdout, "fucked, I'm doomed.\n");
    close(fd);

    return 1;
}
