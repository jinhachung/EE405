/* *****************************************************************************
 * Pre-lab report for Lab 3, Question #5                                       *
 * Keyboard_Control_TMR.c, controlling a TMR with keyboard inputs              *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "gpio_control.h"

#define eof 4
#define esc 27

// inform compiler about arguments and return values
char getch(void);
// functions in gpio_control.c do not need to be listed here because it has a headerfile

int main(void) {
    // 0. print title
    fprintf(stdout, "====== Controlling TMR movement with Keyboard Inputs ======\n");
    // variables
    DIR *d;
    struct dirent *dir;
    char *bone_capemgr = calloc(15, sizeof(char));
    char *abs_bone_capemgr = calloc(28, sizeof(char));
    int found_bone_capemgr = 0;
    int found_ocp = 0;
    int found_pwm0_dir = 0;
    int found_pwm1_dir = 0;
    int found_pwm2_dir = 0;
    int how_many_slots = 0;
    int fd30, fd31;
    unsigned int last_written_led_30 = 0u;
    unsigned int last_written_led_31 = 0u;
    int fd_run_0, fd_period_0, fd_duty_0;
    int fd_run_1, fd_period_1, fd_duty_1;
    int fd_run_2, fd_period_2, fd_duty_2;
    int duty_ns0, duty_ns1, duty_ns2;
    char *duty_ns_str0 = calloc(8, sizeof(char));
    char *duty_ns_str1 = calloc(8, sizeof(char));
    char *duty_ns_str2 = calloc(8, sizeof(char));
    char *pwm0_dir_name = calloc(18, sizeof(char));
    char *pwm1_dir_name = calloc(18, sizeof(char));
    char *pwm2_dir_name = calloc(18, sizeof(char));
    char *abs_pwm0_dir_name = calloc(38, sizeof(char));
    char *abs_pwm1_dir_name = calloc(38, sizeof(char));
    char *abs_pwm2_dir_name = calloc(38, sizeof(char));
    char *afp_pwm0_run = calloc(41, sizeof(char));
    char *afp_pwm0_period = calloc(44, sizeof(char));
    char *afp_pwm0_duty = calloc(42, sizeof(char));
    char *afp_pwm1_run = calloc(41, sizeof(char));
    char *afp_pwm1_period = calloc(44, sizeof(char));
    char *afp_pwm1_duty = calloc(42, sizeof(char));
    char *afp_pwm2_run = calloc(41, sizeof(char));
    char *afp_pwm2_period = calloc(44, sizeof(char));
    char *afp_pwm2_duty = calloc(42, sizeof(char));
    // extra initialization of variables required
    strncpy(abs_bone_capemgr, "/sys/devices/", 13);
    strncpy(abs_pwm0_dir_name, "/sys/devices/", 13);
    strncpy(abs_pwm1_dir_name, "/sys/devices/", 13);
    strncpy(abs_pwm2_dir_name, "/sys/devices/", 13);

    // 1. compute parameters
    double pi = acos((double)-1); // sometimes M_PI is not defined, so use arc-cosine function
    // L: 66mm (0.066m), r: 20mm (0.02m)
    double L = 0.066;
    double r = 0.020;
    // input-managed integer values (-100~100)
    int ivx = 0;
    int ivy = 0;
    int iw  = 0;
    // robot movement velocity (m/s)
    double vx = (double)0;
    double vy = (double)0;
    // robot rotational velocity (rad/s)
    double wr = (double)0;
    // wheel rotational velocities (rad/s)
    double w1 = (double)0;
    double w2 = (double)0;
    double w3 = (double)0;
    // gain_v and gain_w for converting ivx, ivy, and iw to vx, vy, and wr
    double gain_v = (5 * sqrt(3) * pi * r) / 6;
    double gain_w = (5 * pi * r) / (3 * L);

    // note that steps 2 and 3 are same as those implemented in question 4
    // for more specific information on the two steps,
    // check Control_PWM0_Servo.c or Control_Triple_PWM_Servos.c
    // 2. check PWMs are acquired
    d = opendir("/sys/devices");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // we'll need to find /sys/devices/ocp.N later... might as well do it not
            if ((strstr(dir->d_name, "ocp.") == dir->d_name) && (found_ocp == 0)) {
                // set for PWM0
                strncat(abs_pwm0_dir_name, dir->d_name, 5);
                strncat(abs_pwm0_dir_name, "/", 1);
                // set for PWM1
                strncat(abs_pwm1_dir_name, dir->d_name, 5);
                strncat(abs_pwm1_dir_name, "/", 1);
                // set for PWM2
                strncat(abs_pwm2_dir_name, dir->d_name, 5);
                strncat(abs_pwm2_dir_name, "/", 1);
                found_ocp = 1;
            }
            // check if it's the directory we're looking for
            if (strstr(dir->d_name, "bone_capemgr.") == dir->d_name) {
                strncpy(bone_capemgr, dir->d_name, 14);
                // append to absolute path
                strncat(abs_bone_capemgr, bone_capemgr, 14);
                found_bone_capemgr = 1;
                break;
            }
        }
        if (found_bone_capemgr == 0) {
            fprintf(stderr, "cannot find /sys/devices/bone_capemgr.N...\n");
            exit(-1);
        }
        
    }
    else {
        fprintf(stderr, "cannot find directory /sys/devices...\n");
        exit(-1);
    }
    closedir(d);
    // now bone_capemgr.N has been found
    // look for files that start with "slot-" and count how many there are
    d = opendir(abs_bone_capemgr);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // check if it's the file we're looking for
            if (strstr(dir->d_name, "p") == dir->d_name) {
                fprintf(stdout, "%s\n", dir->d_name);
                how_many_slots++;
            }
        }
    }
    else {
        fprintf(stderr, "cannot find directory %s...\n", abs_bone_capemgr);
        exit(-1);
    }
    // appropriate behavior depending on number of slots
    switch (how_many_slots)
    {
    case 0:
        fprintf(stdout, "Please acquire PWMSS and PWM0A first.\n");
        return -1;
        break;
    case 1:
    case 2:
    case 3:
        fprintf(stdout, "Please acquire all of PWM0A, PWM1A, and PWM2A first.\n");
        return -1;
        break;
    case 4:
        // acquired correctly!
        break;
    default:
        // 5+
        fprintf(stderr, "A weird number of 'slot-N' files in %s...\n", abs_bone_capemgr);
        exit(-1);
        break;
    }

    // 3. init PWMs
    if (found_ocp == 0) {
        fprintf(stderr, "cannot find directory /sys/devices...\n");
        exit(-1);
    }
    d = opendir(abs_pwm0_dir_name); // right now, this variable is "/sys/devices/ocp.N/"
    if (d) {
        // ==================== PWM0 ====================
        while ((dir = readdir(d)) != NULL) {
           // check if it's the directory we're looking for
            if (strstr(dir->d_name, "pwm_test_p9_31.") == dir->d_name) {
                strncpy(pwm0_dir_name, dir->d_name, 17);
                // append to absolute path
                strncat(abs_pwm0_dir_name, pwm0_dir_name, 17);
                strncat(abs_pwm0_dir_name, "/", 1);
                found_pwm0_dir = 1;
                break;
            }
        }
        if (found_pwm0_dir == 0) {
            fprintf(stderr, "cannot find /sys/devices/ocp.3/pwm_test_p9_31.xx...\n");
            exit(-1);
        }
        // ==================== PWM1 ====================
        while ((dir = readdir(d)) != NULL) {
           // check if it's the directory we're looking for
            if (strstr(dir->d_name, "pwm_test_p9_14.") == dir->d_name) {
                strncpy(pwm0_dir_name, dir->d_name, 17);
                // append to absolute path
                strncat(abs_pwm1_dir_name, pwm1_dir_name, 17);
                strncat(abs_pwm1_dir_name, "/", 1);
                found_pwm1_dir = 1;
                break;
            }
        }
        if (found_pwm1_dir == 0) {
            fprintf(stderr, "cannot find /sys/devices/ocp.3/pwm_test_p9_14.xx...\n");
            exit(-1);
        }
        // ==================== PWM2 ====================
        while ((dir = readdir(d)) != NULL) {
           // check if it's the directory we're looking for
            if (strstr(dir->d_name, "pwm_test_p8_19.") == dir->d_name) {
                strncpy(pwm2_dir_name, dir->d_name, 17);
                // append to absolute path
                strncat(abs_pwm2_dir_name, pwm2_dir_name, 17);
                strncat(abs_pwm2_dir_name, "/", 1);
                found_pwm2_dir = 1;
                break;
            }
        }
        if (found_pwm2_dir == 0) {
            fprintf(stderr, "cannot find /sys/devices/ocp.3/pwm_test_p8_19.xx...\n");
            exit(-1);
        }
    }
    else {
        fprintf(stderr, "cannot find directory %s...\n", "/sys/devices/ocp.3");
        exit(-1);
    }
    // abs_pwm0_dir_name now saves "/sys/devices/ocp.3/pwm_test_p9_31.xx/"
    // abs_pwm1_dir_name now saves "/sys/devices/ocp.3/pwm_test_p9_14.xx/"
    // abs_pwm2_dir_name now saves "/sys/devices/ocp.3/pwm_test_p8_19.xx/"
    // ==================== set name for run file in PWM0 ====================
    strncpy(afp_pwm0_run, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_run, "run", 3);
    strncpy(afp_pwm0_period, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_period, "period", 6);
    strncpy(afp_pwm0_duty, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_duty, "duty", 4);
    // ==================== set name for run file in PWM1 ====================
    strncpy(afp_pwm1_run, abs_pwm1_dir_name, strlen(abs_pwm1_dir_name));
    strncat(afp_pwm1_run, "run", 3);
    strncpy(afp_pwm1_period, abs_pwm1_dir_name, strlen(abs_pwm1_dir_name));
    strncat(afp_pwm1_period, "period", 6);
    strncpy(afp_pwm1_duty, abs_pwm1_dir_name, strlen(abs_pwm1_dir_name));
    strncat(afp_pwm1_duty, "duty", 4);
    // ==================== set name for run file in PWM2 ====================
    strncpy(afp_pwm2_run, abs_pwm2_dir_name, strlen(abs_pwm2_dir_name));
    strncat(afp_pwm2_run, "run", 3);
    strncpy(afp_pwm2_period, abs_pwm2_dir_name, strlen(abs_pwm2_dir_name));
    strncat(afp_pwm2_period, "period", 6);
    strncpy(afp_pwm2_duty, abs_pwm2_dir_name, strlen(abs_pwm2_dir_name));
    strncat(afp_pwm2_duty, "duty", 4);
    // ==================== init for PWM0 ====================
    // "echo 0 > run"
    fd_run_0 = open(afp_pwm0_run, O_RDWR);
    write(fd_run_0, "0", 1);
    close(fd_run_0);
    // "echo 3000000 > period"
    fd_period_0 = open(afp_pwm0_period, O_RDWR);
    write(fd_period_0, "3000000", 7);
    close(fd_period_0);
    // "echo 1 > run"
    fd_run_0 = open(afp_pwm0_run, O_RDWR);
    write(fd_run_0, "1", 1);
    close(fd_run_0);
    // ==================== init for PWM1 ====================
    // "echo 0 > run"
    fd_run_1 = open(afp_pwm1_run, O_RDWR);
    write(fd_run_1, "0", 1);
    close(fd_run_1);
    // "echo 3000000 > period"
    fd_period_1 = open(afp_pwm1_period, O_RDWR);
    write(fd_period_1, "3000000", 7);
    close(fd_period_1);
    // "echo 1 > run"
    fd_run_1 = open(afp_pwm1_run, O_RDWR);
    write(fd_run_1, "1", 1);
    close(fd_run_1);
    // ==================== init for PWM2 ====================
    // "echo 0 > run"
    fd_run_2 = open(afp_pwm2_run, O_RDWR);
    write(fd_run_2, "0", 1);
    close(fd_run_2);
    // "echo 3000000 > period"
    fd_period_2 = open(afp_pwm2_period, O_RDWR);
    write(fd_period_2, "3000000", 7);
    close(fd_period_2);
    // "echo 1 > run"
    fd_run_2 = open(afp_pwm2_run, O_RDWR);
    write(fd_run_2, "1", 1);
    close(fd_run_2);
    
    // 4. print key guide
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    Q: Left Light  |   W: Forward    |   E: Right Light  |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    A: Left        |   S: Stop       |   D: Right        |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    Z: Rotate CCW  |   X: Backward   |   C: Rotate CW    |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    
    // 5. loop
    // before we loop, get ready to use LED
    gpio_export(30u);
    gpio_export(31u);
    gpio_set_dir(30u, 1u);
    gpio_set_dir(31u, 1u);
    fd30 = gpio_fd_open(30u);
    fd31 = gpio_fd_open(31u);
    while (1) {
loop_start:
        // A. key input without Enter
        char c = getch();
        // B. 'esc' or 'ctrl-D' key terminates
        if ((c == eof) || (c == esc))
            break;
        // C. vx, vy, and w [%] from key input: dec/inc speed by 10%. do saturation
        switch (c)
        {
        case 's':
            ivx = 0;
            ivy = 0;
            iw = 0;
            break;
        case 'w':
            ivx = (ivx == 100) ? ivx : (ivx + 10);
            break;
        case 'x':
            ivx = (ivx == -100) ? ivx : (ivx - 10);
            break;
        case 'a':
            ivy = (ivy == 100) ? ivy : (ivy + 10);
            break;
        case 'd':
            ivy = (ivy == -100) ? ivy : (ivy - 10);
            break;
        case 'z':
            iw = (iw == 100) ? iw : (iw + 10);
        case 'c':
            iw = (iw == -100) ? iw : (iw - 10);
            break;
        case 'q':
            // left light -> (right light when upside down) GPIO 31
            gpio_fd_set_value(fd31, (1u - last_written_led_31));
            break;
        case 'e':
            // right light -> (left light when upside down) GPIO 30
            gpio_fd_set_value(fd30, (1u - last_written_led_30));
            break;
        default:
            goto loop_start;
            break;
        }
        // D. transform vx, vy, w [%] ->[m/s, rad/s] -> kinematics -> ww_rpm[] -> duty_ns
        // D - a: calculate vx, vy, wr
        vx = gain_v * (double)ivx;
        vy = gain_v * (double)ivy;
        wr = gain_w * (double)iw;
        // D - b: transform vector v [vx vy wr]' to vector w [w1 w2 w3]' with "w = Tv"
        // the values of w1, w2, w3 are directly taken from 
        w1 = (-1 * L * wr - vy) / r;
        w2 = (-1 * L * wr + vy / 2 + sqrt(3) * vx / 2) / r;
        w3 = (-1 * L * wr + vy / 2 - sqrt(3) * vx / 2) / r;
        // E. compensate deadband
        // radian w -> 30w/pi rpm -> apply to compensating deadband equation
        // wheel 0
        if (w1 > 0)
            duty_ns0 = (int)((1.45 + 0.27 * w1 / pi) * 1000000);
        else if (w1 < 0)
            duty_ns0 = (int)((1.55 + 0.27 * w1 / pi) * 1000000);
        else
            duty_ns0 = 1500000;
        // wheel 1
        if (w2 > 0)
            duty_ns1 = (int)((1.45 + 0.27 * w2 / pi) * 1000000);
        else if (w2 < 0)
            duty_ns1 = (int)((1.55 + 0.27 * w2 / pi) * 1000000);
        else
            duty_ns1 = 1500000;
        // wheel 2
        if (w3 > 0)
            duty_ns2 = (int)((1.45 + 0.27 * w3 / pi) * 1000000);
        else if (w3 < 0)
            duty_ns2 = (int)((1.55 + 0.27 * w3 / pi) * 1000000);
        else
            duty_ns2 = 1500000;
        // F. output PWM via sysfs with duty_ns considering pos/neg deadband/gain,
        //    also accurate lights
        // get ready to write to duty files
        fd_duty_0 = open(afp_pwm0_duty, O_RDWR);
        fd_duty_1 = open(afp_pwm1_duty, O_RDWR);
        fd_duty_2 = open(afp_pwm2_duty, O_RDWR);
        // get duty_ns_str's
        snprintf(duty_ns_str0, 7, "%d", duty_ns0);
        snprintf(duty_ns_str1, 7, "%d", duty_ns1);
        snprintf(duty_ns_str2, 7, "%d", duty_ns2);
        // write
        write(fd_duty_0, duty_ns_str0, strlen(duty_ns_str0));
        write(fd_duty_1, duty_ns_str1, strlen(duty_ns_str1));
        write(fd_duty_2, duty_ns_str2, strlen(duty_ns_str2));
        // close to commit change
        close(fd_duty_0);
        close(fd_duty_1);
        close(fd_duty_2);
        // G. print key, vx/vy/w, duty_ns
        fprintf(stdout, "vx: %g(m/s), vy: %g(m/s), wr: %g(rad/s)\n", vx, vy, wr);
        fprintf(stdout, "duty0, 1, 2: %d, %d, %d(ns)\n", duty_ns0, duty_ns1, duty_ns2);
        fprintf(stdout, "===========================================================\n");
        // H. usleep(1ms)
        usleep(1000);
    }
    // before exit, stop PWMs
    fd_run_0 = open(afp_pwm0_run, O_RDWR);
    fd_run_1 = open(afp_pwm1_run, O_RDWR);
    fd_run_2 = open(afp_pwm2_run, O_RDWR);
    write(fd_run_0, "0", 1);
    write(fd_run_1, "0", 1);
    write(fd_run_2, "0", 1);
    close(fd_run_0);
    close(fd_run_1);
    close(fd_run_2);
    // before exit, clean up LED usage
    gpio_fd_close(fd30);
    gpio_fd_close(fd31);
    gpio_set_dir(30u, 0u);
    gpio_set_dir(31u, 0u);
    gpio_unexport(30u);
    gpio_unexport(31u);
    // before exit, free all allocated pointers
    free(bone_capemgr);
    free(abs_bone_capemgr);
    free(duty_ns_str0);
    free(pwm0_dir_name);
    free(abs_pwm0_dir_name);
    free(afp_pwm0_run);
    free(afp_pwm0_period);
    free(afp_pwm0_duty);
    free(duty_ns_str1);
    free(pwm1_dir_name);
    free(abs_pwm1_dir_name);
    free(afp_pwm1_run);
    free(afp_pwm1_period);
    free(afp_pwm1_duty);
    free(duty_ns_str2);
    free(pwm2_dir_name);
    free(abs_pwm2_dir_name);
    free(afp_pwm2_run);
    free(afp_pwm2_period);
    free(afp_pwm2_duty);
    fprintf(stdout, "===========================================================\n");
    return 0;
}