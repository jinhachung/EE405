/* *****************************************************************************
 * Pre-lab report for Lab 2, Question #4                                       *
 * gpio_control.h with functions that control the GPIO                         *
 * Names of functions are quite intuitive                                      *
 * to be used with gpio_control.c, where functions are actually implemented    *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#define MAX_BUF 64 // for max length of string

int gpio_export(unsigned int gpio);                     // gpio == gpio number (0 to 127)
int gpio_unexport(unsigned int gpio);                   // gpio == gpio number (0 to 127)
int gpio_set_dir(unsigned int gpio, unsigned int out);  // out == 0 -> in, out == 1 -> out

int gpio_fd_open(unsigned int gpio);                    // returns file descriptor
int gpio_fd_set_value(int fd, unsigned int value);      // value == 0 or 1
int gpio_fd_get_value(int fd, unsigned int *value);     // *value == 0 or 1
int gpio_fd_close(int fd);                              // close for given fd


