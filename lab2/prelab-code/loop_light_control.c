#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>
#include "gpio_control.h"

int main(void) {
    // variables
    struct timeval start_tv, stop_tv;
    struct timezone tz;
    double start_time_in_ns, stop_time_in_ns, elapsed_time_in_ns;
    // 0. print title
    fprintf(stdout, "============= C code for loop - light control =============\n");
    // 1. export: get access permission for GPIO 30 & GPIO 31
    gpio_export(30u);
    gpio_export(31u);
    // 2. set directions of GPIO 30 & GPIO 31 as output
    gpio_set_dir(30u, 1u);
    gpio_set_dir(31u, 1u);
    // 3. get start time (ns) - get us and then multiply by 1000
    gettimeofday(&start_tv, &tz);
    start_time_in_ns = start_tv.tv_sec * 1e9 + start_tv.tv_usec * 1e3;
    // 5. finite loop many times
    // 5 - a: open before we begin loop
    int fd30 = gpio_fd_open(30u);
    int fd31 = gpio_fd_open(31u);
    // 5 - b: finite loop many times -> 100 times
    for (int i = 0; i < 100; ++i) {
        // A. turn on light 1
        gpio_fd_set_value(fd30, 1u);
        // B. turn on light 2
        gpio_fd_set_value(fd31, 1u);
        // C. turn off light 1
        gpio_fd_set_value(fd30, 0u);
        // D. turn off light 2
        gpio_fd_set_value(fd31, 0u);
    }
    // 6. get end time (ns)
    gettimeofday(&stop_tv, &tz);
    stop_time_in_ns = stop_tv.tv_sec * 1e9 + stop_tv.tv_usec * 1e3;
    elapsed_time_in_ns = stop_time_in_ns - start_time_in_ns;
    // 7. echo end/start time
    fprintf(stdout, "================= looped 100 times to get =================\n");
    fprintf(stdout, "start time: %gns\n", start_time_in_ns);
    fprintf(stdout, " stop time: %gns\n", stop_time_in_ns);
    fprintf(stdout, "time taken: %gns in 100 loops\n", elapsed_time_in_ns);
    fprintf(stdout, "average time: %gns per loop\n", elapsed_time_in_ns / 100);
    fprintf(stdout, "===========================================================\n");
    // 8. set directions as input
    // 8 - a: close
    gpio_fd_close(fd30);
    gpio_fd_close(fd31);
    // 8 - b: set direction
    gpio_set_dir(30u, 0u);
    gpio_set_dir(31u, 0u);
    // 9. unexport: release access permission for GPIO 30 & GPIO 31
    gpio_unexport(30u);
    gpio_unexport(31u);

    return 1;
}
