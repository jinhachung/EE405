/* Test_LightLEDs_Module.c: based off Test_HelloDev.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc, char **argv)
{
	int fd, rc;
    unsigned int user_input;
    char *rd_buf[16];
	// 1. open LightLEDs device open("/dev/LightLEDs", ...)
	fd = open("/dev/LightLEDs", O_RDWR);
	if (fd == -1) {
        // failed open
		perror("open failed");
		rc = fd;
		exit(-1);
	}
	fprintf(stdout, "%s device open success.\n", argv[0]);
    // 2. user interface loop
    while (1) {
        fprintf(stdout, "Enter data for %s (0 to 3): ");
        fflush(stdout);
        // A. get user data (0 to 3, neg to exit)
        // A-a: get data
        rc = read(0, rd_buf, 2);    // fd == 0 is stdin
        // set user_input properly
        if (rc == 1) {
            if (rd_buf[0] == '0')
                user_input = 0u;
            else if (rd_buf[0] == '1')
                user_input = 1u;
            else if (rd_buf[0] == '2')
                user_input = 2u;
            else if (rd_buf[0] == '3')
                user_input = 3u;
            else {
                close(fd);  // invalid used input... close and exit
                exit(-1);   // exit
            }
        }
        // A-b: exit loop of user_data < 0]
        if ((rc == 2) && (rd_buf[0] == '-'))
            break;
        else {
            close(fd);  // invalid user input... close and exit
            exit(-1);   //exit
        }
        // B. write (dev, user_data)
        write(fd, rd_buf, 1);
        fprintf(stdout, "Write %s with data %d.\n", argv[0], (int)user_input);
    }
	// 3. close LightLEDs device: close()
	close(fd);
    fprintf(stdout, "%s device closed.\n", argv[0]);
	return 0;
}


