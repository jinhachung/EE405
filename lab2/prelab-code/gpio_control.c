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
    // open file in write-only mode
    int fd = open(str, O_WRONLY);
    // write 'in' or 'out' depending on value of variable out
    if (out == 0u) {
        write(fd, "in", 2);
    }
    else if (out == 1u) {
        write(fd, "out", 3);
    }
    else {
        // reaching here means out != 0 && out != 1
        fprintf(stderr, "gpio_set_dir(): invalid out number - out should be 0 or 1\n");
        return -1;
    }
    // always close opened file when done
    gpio_fd_close(fd);
    // free unneeded pointer
    free(str);
    // return 1 for success
    return 1;
}

// as far as I understand, this function should open the VALUE file for the given gpio(XX)
// namely, /sys/class/gpio/gpioXX/value
// and should be called for gpio_fd_set_value() and gpio_fd_get_value() 
// **** it is also checked in this function whether gpio value XX is in the correct range [0, 127] ****
// -1 is returned (invalid value as a file descriptor) if the number is NOT in the right range
// OTHERWISE, the file descriptor for the open file is returned
int gpio_fd_open(unsigned int gpio) {
    size_t len = gpio_check_and_get_length(gpio);
    if (len == (size_t)0) {
        // len == 0 means it was not in the right range -> return -1
        // TODO: should error be raised in the calling function?
        fprintf(stderr, "gpio_fd_open(): invalid gpio number\n");
        return -1;
    }
    // place the gpio number inside file name with snprintf()
    // "/sys/class/gpio/gpioXX/value" is 26 characters (without XX)
    // and +1 for null terminator to get len + 27 bytes of malloc()
    char *str = (char *)malloc(len + 27);
    snprintf(str, len + 27, "/sys/class/gpio/gpio%d/value", gpio);
    // open file in reading & writing mode - we use both 'set' (writing) AND 'get' (reading)
    int fd = open(str, O_RDWR);
    return fd;
    //return open(str, O_RDWR;
}

// write 'value' to the file with file descriptor fd
// if value is NOT 0 nor 1, return -1 witihout writing instead
int gpio_fd_set_value(int fd, unsigned int value) {
    // check if value is valid (0 or 1), and write if valid
    if (value == 0u)
        write(fd, "0", 1);
    else if (value == 1u)
        write(fd, "1", 1);
    else {
        // invalid value -> raise error and return -1
        fprintf(stderr, "gpio_fd_set_value(): invalid value number - value should be 0 or 1'\n");
        return -1;
    }
    // reaching here means successfully written
    // TODO: should it return 1 for success or return the 'value' written?
    return 1;
    // return (int)value;
}

// read 'value' from the file with file descriptor fd
int gpio_fd_get_value(int fd, unsigned int *value) {
    char *buffer = (char *)calloc(10, sizeof(char)); // 10 bytes should be enough to store 1 byte
    int numbytes = (int)read(fd, buffer, 5); // read 5 bytes
    // numbytes should be 1, and the read data must be either 0 or 1
    if (numbytes == 1) {
        // value must be 0 or 1, otherwise file is corrupted
        if (buffer[0] == '0') {
            *value = 0u;
            // return 1 for success
            return 1;
        }
        else if (buffer[0] == '1') {
            *value = 1u;
            // return 1 for success
            return 1;
        }
    }
    // reaching here means the file has been corrupted - value stord is NOT 0 nor 1
    fprintf(stderr, "gpio_fd_get_value(): file has been corrupted\n");
    return -1;
}

// close the given file descriptor file
int gpio_fd_close(int fd) {
    // returns 0 on success and -1 on error
    return close(fd);
}
