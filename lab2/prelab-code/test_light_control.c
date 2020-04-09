#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio_control.h"

int main(void) {
    // 0. print title
    fprintf(stdout, "============= C code for test - light control =============\n");
    // 1. set variables: light_id = 1;
    int light_id = 1;
    int on_off_int;
    char *on_off_str;
    // 2. export GPIO 30 & GPIO 31
    gpio_export(30u);
    gpio_export(31u);
    //fprintf(stdout, "===========================================================\n");
    // 3. set direction of GPIO 30 & GPIO 31 to out, open GPIO 30 & GPIO 31
    // 3 - a: set direction
    gpio_set_dir(30u, 1u);
    gpio_set_dir(31u, 1u);
    // 3 - b: open
    int fd30 = gpio_fd_open(30u);
    int fd31 = gpio_fd_open(31u);
    // 5. loop while light_id > 0
    while (light_id > 0) {
        // A - a: prompt output "Enter light_id and on_off_str: "
        fprintf(stdout, "Enter light_id and on_off_str: ");
        fflush(stdout);
        // A - b: get user input of light_id and on_off_str
        fscanf(stdin, "%d %s", &light_id, on_off_str);
        // B. check light_id, break if <= 0
        if (light_id <= 0)
            break;
        // C. check on_off_str and set on_off_int
        if (strncmp("on", on_off_str, 3) == 0)
            on_off_int = 1;
        else if (strncmp("off", on_off_str, 3) == 0)
            on_off_int = 0;
        else // break if on_off_str is not 'on' nor 'off'
            break;
        // D. control action
        // 1 is connected to GPIO 30, and 2 is connected to GPIO 31
        if (light_id == 1)
            gpio_fd_set_value(fd30, (unsigned int)on_off_int);
        else if (light_id == 2)
            gpio_fd_set_value(fd31, (unsigned int)on_off_int);
        else
            break;
    }
    // 8. close GPIO 30 & 31 and set direction to input
    // 8 - a: close
    gpio_fd_close(fd30);
    gpio_fd_close(fd31);
    // 8 - b: set direction
    gpio_set_dir(30u, 0u);
    gpio_set_dir(31u, 0u);
    // 9. unexport GPIO 30 & GPIO 31
    gpio_unexport(30u);
    gpio_unexport(31u);

    return 1;
}