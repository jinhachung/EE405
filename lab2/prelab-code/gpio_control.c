/* *****************************************************************************
 * Pre-lab report for Lab 2, Question #4                                       *
 * gpio_control.c with functions that control the GPIO                         *
 * Names of functions are quite intuitive                                      *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include "gpio_control.h"

int gpio_export(unsigned int gpio) {
    // check if gpio is in the write range (0~127)
    if (!((gpio >= 0u) && (gpio <= 127u))) {
        fprintf(stderr, "gpio_export(): invalid gpio number\n");
        return -1;
    }
    // now that gpio falls in the valid range, find out how long it is
    // since 0 <= gpio <= 128, if gpio <= 10, it is 1 digit
    // else if gpio <= 100, it is 2 digits, and else 3 digits
    size_t len = gpio < 10u ? 1 : (gpio < 100u ? 2 : 3);
    // now we know the precise length of gpio (in digits), convert it to string
    // better not itoa() since it is not a standard
    // reference: https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
    char *str = (char *)malloc(len + 1); // allocate one more space for null terminator
    snprintf(str, len + 1, "%d", gpio); // no need to worry about %d because 0 <= gpio <= 127
    // open file in write-only mode
    int fd = open("/sys/class/gpio/export", O_WRONLY);
    write(fd, str, len); // no need for +1 to len because we don't need to write the null-terminator
    // always close opened file when done
    close(fd);
    // free unneeded pointer
    free(str);
    // return 1 for success
    return 1;
}

int gpio_unexport(unsigned int gpio) {
    // this function does the same thing as previously implemented gpio_export()
    // except instead of writing to /sys/class/gpio/export, we need to write to /sys/class/gpio/unexport
    // check if gpio is in the right range
    if (!((gpio >= 0u) && (gpio <= 127u))) {
        fprintf(stderr, "gpio_export(): invalid gpio number\n");
        return -1;
    }
    // set digit length for gpio
    size_t len = gpio < 10u ? 1 : (gpio < 100u ? 2 : 3);
    // copy into string buffer
    char *str = (char *)malloc(len + 1); // allocate one more space for null terminator
    snprintf(str, len + 1, "%d", gpio); // no need to worry about %d because 0 <= gpio <= 127
    // open file in write-only mode
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, str, len); // no need for +1 to len because we don't need to write the null-terminator
    // always close opened file when done
    close(fd);
    // free unneeded pointer
    free(str);
    // return 1 for success
    return 1;
} 

int gpio_set_dir(unsigned int gpio, unsigned int out) {
    ;
}

int gpio_fd_open(unsigned int gpio) {
    ;
}
int gpio_fd_set_value(int fd, unsigned int value) {
    ;
}

int gpio_fd_get_value(int fd, unsigned int *value) {
    ;
}

int gpio_fd_close(int fd) {
    ;
}
