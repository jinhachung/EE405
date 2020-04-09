/* *****************************************************************************
 * Pre-lab report for Lab 2, Question #4                                       *
 * gpio_control.c with functions that control the GPIO                         *
 * Names of functions are quite intuitive                                      *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>      // for open()
#include <sys/stat.h>       // for open()
#include <fcntl.h>          // for open()
#include <unistd.h>         // for close()
#include "gpio_control.h"   // header file

// this function checks the input gpio value
// if gpio is in the correct range (0 <= gpio <= 127),
// return its length in decimal (0~9: 1, 10~99:2, 100~127:3)
// if it is not in its correct range, return 0
size_t gpio_check_and_get_length(unsigned int gpio) {
    // check if gpio is in the right range
    if (!((gpio >= 0u) && (gpio <= 127u)))
        return (size_t)0;
    // now that gpio falls in the valid range, find out how long it is
    // since 0 <= gpio <= 128, if gpio <= 10, it is 1 digit
    // else if gpio <= 100, it is 2 digits, and else 3 digits
    size_t len = gpio < 10u ? 1 : (gpio < 100u ? 2 : 3);
    return len;
}

int gpio_export(unsigned int gpio) {
    size_t len = gpio_check_and_get_length(gpio);
    if (len == (size_t)0) {
        // len == 0 means it was not in the right range -> raise error and return -1
        fprintf(stderr, "gpio_export(): invalid gpio number\n");
        return -1;
    }
    // convert the number gpio into string
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

// this function does the same thing as previously implemented gpio_export()
// except instead of writing to /sys/class/gpio/export, we need to write to /sys/class/gpio/unexport
int gpio_unexport(unsigned int gpio) {
    size_t len = gpio_check_and_get_length(gpio);
    if (len == (size_t)0) {
        // len == 0 means it was not in the right range -> raise error and return -1
        fprintf(stderr, "gpio_export(): invalid gpio number\n");
        return -1;
    }
    // convert the number gpio into string
    char *str = (char *)malloc(len + 1); // allocate one more space for null terminator
    snprintf(str, len + 1, "%d", gpio); // no need to worry about %d because 0 <= gpio <= 127
    // open file in write-only mode
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd, str, len); // no need for +1 to len because we don't need to write the null-terminator
    // always close opened file when done
    gpio_fd_close(fd);
    // free unneeded pointer
    free(str);
    // return 1 for success
    return 1;
}

int gpio_set_dir(unsigned int gpio, unsigned int out) {
    size_t len = gpio_check_and_get_length(gpio);
    if (len == (size_t)0) {
        // len == 0 means it was not in the right range -> raise error and return -1
        fprintf(stderr, "gpio_export(): invalid gpio number\n");
        return -1;
    }
    // place the gpio number inside file name with snprintf()
    // "/sys/class/gpio/gpioXX/direction" is 30 characters (without XX)
    // and +1 for null terminator to get len + 31 bytes of malloc()
    char *str = (char *)malloc(len + 31);
    snprintf(str, len + 31, "/sys/class/gpio/gpio%d/direction", gpio);
    int fd = open();
}

// as far as I understand, this function should open the VALUE file for the given gpio(XX)
// namely, /sys/class/gpio/gpioXX/value
// and should be called for gpio_fd_set_value() and gpio_fd_get_value() 
// it should return the file descriptor for the opened file
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
    // close the given file descriptor file
    close(fd);

    return fd;
}
