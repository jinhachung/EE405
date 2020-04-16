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
    int fd_run_0, fd_period_0, fd_duty_0;
    int fd_run_1, fd_period_1, fd_duty_1;
    int fd_run_2, fd_period_2, fd_duty_2;
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
    int ivx = 0;
    int ivy = 0;
    int iw = 0;

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
    fd_run_0 = open(afp_pwm0_run);
    write(fd_run_0, "0", 1);
    close(fd_run_0);
    // "echo 3000000 > period"
    fd_period_0 = open(afp_pwm0_period);
    write(fd_period_0, "3000000", 7);
    close(fd_period_0);
    // "echo 1 > run"
    fd_run_0 = open(afp_pwm0_run);
    write(fd_run_0, "1", 1);
    close(fd_run_0);
    // ==================== init for PWM1 ====================
    // "echo 0 > run"
    fd_run_1 = open(afp_pwm1_run);
    write(fd_run_1, "0", 1);
    close(fd_run_1);
    // "echo 3000000 > period"
    fd_period_1 = open(afp_pwm1_period);
    write(fd_period_1, "3000000", 7);
    close(fd_period_1);
    // "echo 1 > run"
    fd_run_1 = open(afp_pwm1_run);
    write(fd_run_1, "1", 1);
    close(fd_run_1);
    // ==================== init for PWM2 ====================
    // "echo 0 > run"
    fd_run_2 = open(afp_pwm2_run);
    write(fd_run_2, "0", 1);
    close(fd_run_2);
    // "echo 3000000 > period"
    fd_period_2 = open(afp_pwm2_period);
    write(fd_period_2, "3000000", 7);
    close(fd_period_2);
    // "echo 1 > run"
    fd_run_2 = open(afp_pwm2_run);
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
    while (1) {
        // A. key input without Enter
        char c = getch();
        // B. 'esc' or 'ctrl-D' key terminates
        if ((c == eof) || (c == esc))
            break;
        // C. vx, vy, and w [%] from key input: Dec/inc speed by 10%. do saturation
        ;
        // D. transform vx, vy, w [%] ->[m/s, rad/s] -> kinematics -> ww_rpm[] -> duty_ns
        ;
        // E. compensate deadband
        ;
        // F. output PWM via sysfs with duty_ns considering pos/neg deadband/gain,
        //    also accurate lights
        ;
        // G. print key, vx/vy/w, duty_ns
        ;
        // H. usleep(1ms)
        usleep(1000);
        ;
    }

    fprintf(stdout, "===========================================================\n");
    return 0;
}