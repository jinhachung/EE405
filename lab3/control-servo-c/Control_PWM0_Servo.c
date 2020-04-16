/* *****************************************************************************
 * Pre-lab report for Lab 3, Question #4                                       *
 * Control_PWM0_Servo.c, C-version of question #3's shell script               *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    // 0. print title
    fprintf(stdout, "=========== C code for control of PWM0 on Servo ===========\n");
    // 1. check if PWM0 acquired
    DIR *d;
    struct dirent *dir;
    int duty_us, duty_ns; // user input of duty value from stdin
    char *duty_ns_str = calloc(8, sizeof(char)); // "1500000" is 8 letters including null-terminator
    char *bone_capemgr = calloc(15, sizeof(char)); // bone_capemgr.N is 15 letters including null-terminator
    char *abs_bone_capemgr = calloc(28, sizeof(char)); // absolute path to bone_capemgr.N, "/sys/devices/bone_capemgr.N"
    int found_bone_capemgr = 0;
    int found_ocp = 0;
    int how_many_slots = 0;
    strncpy(abs_bone_capemgr, "/sys/devices/", 13);
    // PWM0
    char *pwm0_dir_name = calloc(18, sizeof(char)); // "pwm_test_p9_31.xx" is 18 letters including null-terminator
    char *abs_pwm0_dir_name = calloc(38, sizeof(char)); // absolute path to pwm_test_p9_31.xx, "/sys/devices/ocp.3/pwm_test_p9_31.xx/"
    // afp (absolute file path) to different files for PWM0 directory
    char *afp_pwm0_run = calloc(41, sizeof(char));
    char *afp_pwm0_period = calloc(44, sizeof(char));
    char *afp_pwm0_duty = calloc(42, sizeof(char));
    int found_pwm0_dir = 0;
    // file descriptors for PWM0 
    int fd_run_0, fd_period_0, fd_duty_0;
    // we will find /sys/devices/ocp.N later and strncat "ocp.N/" here
    strncpy(abs_pwm0_dir_name, "/sys/devices/", 13);

    // check directory /sys/devices/ for a directory that starts with "bone_capemgr."
    // this part was borrowed from Jean-Bernard Jansen's answer on:
    // https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
    d = opendir("/sys/devices");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // we'll need to find /sys/devices/ocp.N later... might as well do it not
            if ((strstr(dir->d_name, "ocp.") == dir->d_name) && (found_ocp == 0)) {
                strncat(abs_pwm0_dir_name, dir->d_name, 5);
                strncat(abs_pwm0_dir_name, "/", 1);
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
    // 0    -> not acquired
    // 1    -> acquired only overall PWM
    // 2, 3 -> unsure... ASSUME PWM0A is acquired
    // 4    -> acquired!
    // 5+   -> unsure... exit(-1)
    switch (how_many_slots)
    {
    case 0:
        fprintf(stdout, "Please acquire PWMSS and PWM0A first.\n");
        return -1;
        break;
    case 1:
        fprintf(stdout, "Please acquire PWM0A first.\n");
        return -1;
        break;
    case 2:
        // since we assume PWM0A is acquired for this, go to case 4
    case 3:
        // since we assume PWM0A is acquired for this, go to case 4
    case 4:
        // acquired correctly!
        break;
    default:
        // 5+
        fprintf(stderr, "A weird number of 'slot-N' files in %s...\n", abs_bone_capemgr);
        exit(-1);
        break;
    }
    // reaching here means PWM0A has been acquired correctly
    // 2. init PWM0A via sysfs
    // PWM0A is a directory in the form of "/sys/devices/ocp.3/pwm_test_p9_31.xx"
    if (found_ocp == 0) {
        fprintf(stderr, "cannot find directory /sys/devices...\n");
        exit(-1);
    }
    d = opendir(abs_pwm0_dir_name); // right now, this variable is "/sys/devices/ocp.N/"
    if (d) {
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
    }
    else {
        fprintf(stderr, "cannot find directory %s...\n", "/sys/devices/ocp.3");
        exit(-1);
    }
    // abs_pwm0_dir_name now saves "/sys/devices/ocp.3/pwm_test_p9_31.xx/"
    // set name for run file in PWM0
    strncpy(afp_pwm0_run, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_run, "run", 3);
    strncpy(afp_pwm0_period, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_period, "period", 6);
    strncpy(afp_pwm0_duty, abs_pwm0_dir_name, strlen(abs_pwm0_dir_name));
    strncat(afp_pwm0_duty, "duty", 4);

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

    // 5. loop
    while (1) {
        // A. user input of duty_us (1000 to 2000)
        fprintf(stdout, "Enter duty (in us, from 1000 to 2000): ");
        fflush(stdout);
        fscanf(stdin, "%d", &duty_us);
        // B. break if duty_us < 0
        if ((duty_us < 1000) || (duty_us > 2000)) 
            break;
        // open duty file because now, we will DEFINITELY write to this file
        fd_duty_0 = open(afp_pwm0_duty);
        // C. convert linearly from duty_us to duty_ns, and get duty_ns_str's
        // C - a: convert duty_us to duty_ns
        duty_ns = duty_us * 1000;
        // D. compensate deadband
        // assume deadband range 1.45ms ~ 1.55ms
        // then for input duty, the actual duty is as follows
        /*
        if (duty_ns < 1500000)
            duty_ns = 0.9 * duty_ns + 100000;
        else if (duty_ns > 1500000)
            duty_ns = 0.9 * duty_ns + 200000;
        */
        if (duty_ns != 1500000)
            duty_ns = 0.9 * duty_ns + 100000 * ((duty_ns > 1500000) ? 2 : 1);
        // C - b: get duty_ns_str
        snprintf(duty_ns_str, 7, "%d", duty_ns);
        // E. control PWM with duty_ns_str
        // "echo $duty_ns > duty"
        write(fd_duty_0, duty_ns_str, strlen(duty_ns_str));
        // close to commit change
        close(fd_duty_0);
    }
    // 8. stop PWM0A
    // "echo 0 > run"
    fd_run_0 = open(afp_pwm0_run);
    write(fd_run_0, "0", 1);
    
    // 9. close files
    close(fd_run_0);

    // before exit, free all allocated pointers
    free(duty_ns_str);
    free(bone_capemgr);
    free(abs_bone_capemgr);
    // free for PWM0
    free(pwm0_dir_name);
    free(abs_pwm0_dir_name);
    free(afp_pwm0_run);
    free(afp_pwm0_period);
    free(afp_pwm0_duty);
    //fprintf(stdout, "===========================================================\n");
    return 0;
}
