/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
/* For network connection */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

/******************************************************************************/
/* DEFINES                                                                    */
/******************************************************************************/

#define UDP_PORT 12345

#define MAX(a,b) ((a)>(b))?(a):(b)
#define MIN(a,b) ((a)<(b))?(a):(b)

/******************************************************************************/
/* CONSTANTS                                                                  */
/******************************************************************************/

const int kPwmRange = 500000; // ns
const float kWheelRadius = 0.05; // m
const float kWheelLength = 0.10; // m
const float kRoot3 = 1.73205081;

/* dead-band pwm duty for eac servo motor */
const int kDeadbandUpperPWM0A = 100000; // ns
const int kDeadbandLowerPWM0A = 100000; // ns
const int kDeadbandUpperPWM1A = 100000; // ns
const int kDeadbandLowerPWM1A = 100000; // ns
const int kDeadbandUpperPWM2A = 100000; // ns
const int kDeadbandLowerPWM2A = 100000; // ns

const float gain_v = 1.0;
const float gain_w = 1.0;

/* pwm directory path for beaglebone */
const char* pwm_path[3] = {
  "/sys/devices/ocp.3/pwm_test_P9_22.15",
  "/sys/devices/ocp.3/pwm_test_P9_14.16",
  "/sys/devices/ocp.3/pwm_test_P8_19.17"
};

/******************************************************************************/
/* FUNCTION DECLARATIONS                                                      */
/******************************************************************************/

/* getch() from getch.c file */
extern char getch(void);

static int CheckPwmDir(void);
static int InitPwm(void);
static int InitLight(void);
static int ReleasePwm(void);
static int ReleaseLight(void);
static void echo(char *str, char *file);

/******************************************************************************/
/* MAIN                                                                       */
/******************************************************************************/

int main(int argc, char *argv[])
{
  int i;

  char input;
  int LL, RL;
  int ivx, ivy, iw;

  float vx, vy, wr;

  bool valid_key;

  int ret;
  int option;
  int server_socket;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int server_addr_len;
  int client_addr_len;

  char command[256];
  int nbytes;

  int fd_PWM[3];
  char file[256];
  int pwm_duty[3];
  char pwm_duty_str[3][32];

  int fd_light[2];
  char light_str[2][32];

  system("clear");
  printf("%s\n", argv[0]);

  /* 1. Check PWM directories. */
  CheckPwmDir();
  /* 2. Initialize PWM duties. */
  InitPwm();
  /* 3. Initialize Lights */
  InitLight();
  /* 4. Open PWM duty files */
  for(i=0; i<3; i++) {
	sprintf(file, "%s/%s", pwm_path[i], "duty");
	fd_PWM[i] = open(file, O_RDWR);
	if(fd_PWM[i] < 0)
	{
	  printf("can not open PWM%dA's duty file.\n", i);
	  exit(-1);
	}
  }
  /* 5. Open Light GPIO files */
  fd_light[0] = open("/sys/class/gpio/gpio30/value", O_WRONLY);
  fd_light[1] = open("/sys/class/gpio/gpio31/value", O_WRONLY);

 /* TODO: A. Make server socket using socket() function (UDP socket) */
  server_socket = socket(/* Code */);
  if (server_socket < 0) {
	printf("server: can not open socket.\n");
	return -1;
  }
  option = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(UDP_PORT);
  server_addr_len = sizeof(server_addr);

  /* TODO: B. Bind server socket using bind() function */
  ret = bind(/* Code */);
  if (ret < 0) {
	printf("server: can not bind socket to local IP address.\n");
	return -1;
  }

  while(1)
  {
    /* TODO: C. Receive packet from client using recvfrom() function */
    nbytes = recvfrom(/* Code */);
    command[nbytes] = '\0';
          if (strcmp(command, "exit") == 0) {
            break;
          }
    printf("%s\n", command);

    /* TODO: D. Tokenize command packet using strtok() function */
    pwm_duty[0] = atoi(strtok(/* Code */));
    pwm_duty[1] = atoi(strtok(/* Code */));
    pwm_duty[2] = atoi(strtok(/* Code */));
          LL = atoi(strtok(/* Code */));
          RL = atoi(strtok(/* Code */));
          
    /* Write PWM and Light commands */
    sprintf(pwm_duty_str[0], "%d", pwm_duty[0]);
    sprintf(pwm_duty_str[1], "%d", pwm_duty[1]);
    sprintf(pwm_duty_str[2], "%d", pwm_duty[2]);

    sprintf(light_str[0], "%d", LL);
    sprintf(light_str[1], "%d", RL);

    write(fd_PWM[0], pwm_duty_str[0], strlen(pwm_duty_str[0]));
    write(fd_PWM[1], pwm_duty_str[1], strlen(pwm_duty_str[1]));
    write(fd_PWM[2], pwm_duty_str[2], strlen(pwm_duty_str[2]));

    write(fd_light[0], light_str[0], strlen(light_str[0]));
    write(fd_light[1], light_str[1], strlen(light_str[1]));
  }

  ReleasePwm();
  ReleaseLight();

  close(fd_PWM[0]);
  close(fd_PWM[1]);
  close(fd_PWM[2]);  

  close(fd_light[0]);
  close(fd_light[1]);

  return 0;
}

/******************************************************************************/
/* FUNCTION DEFINITIONS                                                       */
/******************************************************************************/

/* Check if pwm directories exist */
static int CheckPwmDir(void)
{
  int i;
  for(i = 0; i < 3; i++) {
	if(access(pwm_path[i], 0) < 0) {
	  printf("%s not exist.\n",pwm_path[i]);
	  exit(-1);
	}
  }
  return 0;
}

/* Init PWMs via sysfs */
static int InitPwm(void)
{
  int i;
  char file[256];
  for(i = 0; i < 3; i++)
  {
	sprintf(file, "%s/%s", pwm_path[i], "run");
	echo("0", file);
	sprintf(file, "%s/%s", pwm_path[i], "period");
	echo("3000000", file);
	sprintf(file, "%s/%s", pwm_path[i], "duty");
	echo("1500000", file);
	sprintf(file, "%s/%s", pwm_path[i], "run");
	echo("1", file);
  }
}

static int InitLight(void)
{
  echo("30", "/sys/class/gpio/export");
  echo("31", "/sys/class/gpio/export");
  echo("out", "/sys/class/gpio/gpio30/direction");
  echo("out", "/sys/class/gpio/gpio31/direction");
  echo("0", "/sys/class/gpio/gpio30/value");
  echo("0", "/sys/class/gpio/gpio31/value");
}

static int ReleasePwm(void)
{ 
  int i;
  char file[256];
  /* Stop PWMs */
  for(i = 0; i < 3; i++)
  {
	sprintf(file, "%s/%s", pwm_path[i], "run");
	echo("0", file);
  }
}

static int ReleaseLight(void)
{
  /* Unexport Lights */
  echo("0", "/sys/class/gpio/gpio30/value");
  echo("0", "/sys/class/gpio/gpio31/value");
  echo("in", "/sys/class/gpio/gpio30/direction");
  echo("in", "/sys/class/gpio/gpio31/direction");
  echo("30", "/sys/class/gpio/unexport");
  echo("31", "/sys/class/gpio/unexport");
}

static void echo(char *str, char *file)
{
  int fd = open(file, O_WRONLY);
  if(fd < 0)
  {
	printf("%s:open error.\n", file);
	exit(-1);
  }
  write(fd, str, strlen(str));
  close(fd);
}
