/* *****************************************************************************
 * Pre-lab report for Lab 3, Question #5                                       *
 * Test_Key_Process.c, testing key inputs & correctness of functionalities     *
 * 2020 Spring, EE405 (A) Electronics Design Lab - RoboCam                     *
 * by Jinha Chung, KAIST EE                                                    *
 * *************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
    
    fprintf(stdout, "====== Controlling TMR movement with Keyboard Inputs ======\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    Q: Left Light  |   W: Forward    |   E: Right Light  |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    A: Left        |   S: Stop       |   D: Right        |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "|    Z: Rotate CCW  |   X: Backward   |   C: Rotate CW    |\n");
    fprintf(stdout, "+---------------------------------------------------------+\n");
    fprintf(stdout, "===========================================================\n");
    
    return 0;
}

