#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    int light_id;
    //char *on_off_str = malloc(10);
    char *on_off_str;

    fprintf(stdout, "Enter light_id and on_off_str: ");
    fflush(stdout);
    fscanf(stdin, "%d %s", &light_id, on_off_str);
    fprintf(stdout, "read: %d AND %s\n", light_id, on_off_str);

    if (strncmp("on", on_off_str, 3) == 0)
        fprintf(stdout, "ON IT IS\n");
    else if (strncmp("off", on_off_str, 3) == 0)
        fprintf(stdout, "OFF IT IS\n");
    else
        fprintf(stdout, "oh fuck, I'm doomed...\n");

    //free(on_off_str);
    return 1;
}
