/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
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

/******************************************************************************/
/* FUNCTION DECLARATIONS                                                      */
/******************************************************************************/

/* getch() from getch.c file */
extern char getch(void);

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

  int pwm_duty[3];

  char command[256];
  
  int client_socket;
  struct sockaddr_in server_addr;
  int server_addr_len;

  if (argc != 2) {
    printf("Usage: %s <IP>\n", argv[0]);
    return 0;
  }

  system("clear");
  printf("%s\n", argv[0]);

  /* 1. Initialize UDP socket */
  /* TODO: A. Make client socket using socket() function (UDP socket) */
  client_socket = socket(/* Code */);
  if (client_socket < 0) {
    printf("client: can not open socket.\n");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(UDP_PORT);
  server_addr_len = sizeof(server_addr);

  /* 2. Initialize control variables */
  ivx = ivy = iw = LL = RL = 0;
  
  /* 3. Print Key Map */
  printf("+-----------------------------------+\n");
  printf("|   Q: LL   |  W: +vx   |   E: RL   |\n");
  printf("|-----------+-----------+-----------|\n");
  printf("|   A: -vy  |  S: Stop  |   D: +vy  |\n");
  printf("|-----------+-----------+-----------|\n");
  printf("|   Z: +w   |  X: -vx   |   C: -w   |\n");
  printf("+-----------------------------------+\n");
  
  while(1)
  {
    valid_key = true;
    switch(getch())
    {
      /* TODO: B. Write the code for commands */
      case 'w':
        /* Example. Increase speed x-axis with upper limit */
        ivx = MIN(ivx+1,10);
        break;
      case 'a':
        /* B1. Increase speed y-axis with upper limit (ivy)*/
        break;
      case 's':
        /* B2. Stop */
        break;
      case 'd':
        /* B3. Decrease speed y-axis with lower limit (ivy)*/
        break;
      case 'x':
        /* B4. Decrease speed x-axis with lower limit (ivx)*/
        break;
      case 'z':
        /* B5. Increase rotation speed z-axis with upper limit (iw)*/
        break;
      case 'c'
        /* B6. Decrease rotation speed z-axis with lower limit (iw)*/
        break;
      case 'q':
        /* B7. Toggle Left LED (LL)*/
        break;
      case 'e':
        /* B8. Toggle Right LED (RL)*/
        break;
      case 0x1B: //ESC
        goto done;
        break;
      default:
        valid_key = false;
    }

    if(valid_key)
    {
      /* Convert from input to velocity */
      vx = ivx * 10;
      vy = ivy * 10;
      wr = iw * 10;
     
      system("clear");
      /* Print control value */
      printf("+-----------+-----------+-----------+\n");
      printf("|  vx: %4d |  vy: %4d |   w: %4d |\n", (int)vx, (int)vy, (int)wr);
      printf("|-----------+-----------+-----------|\n");
      printf("|  LL: %s  |           |  RL: %s  |\n", (LL?"ON ":"OFF"),(RL?"ON ":"OFF"));

      /* Convert from velocity to PWM duty */      
      vx = (kPwmRange * gain_v) * vx / 100.0;
      vy = (kPwmRange * gain_v) * vy / 100.0;
      wr = (kPwmRange * gain_w) * wr / 100.0;

      pwm_duty[0] = (-vy) - (kWheelLength*wr);
      pwm_duty[1] = (kRoot3/2.*vx) + (1./2.*vy) - (kWheelLength*wr);
      pwm_duty[2] = (-kRoot3/2.*vx) + (1./2.*vy) - (kWheelLength*wr);

      pwm_duty[0] /= kWheelRadius;
      pwm_duty[1] /= kWheelRadius;
      pwm_duty[2] /= kWheelRadius;

      /* Align rotation direction */
      pwm_duty[0] *= -1;
      pwm_duty[1] *= -1;
      pwm_duty[2] *= -1;
      
      /* Scaling */
      pwm_duty[0] /= ((kRoot3/2.) + (1./2.) + (kWheelLength))/kWheelRadius;
      pwm_duty[1] /= ((kRoot3/2.) + (1./2.) + (kWheelLength))/kWheelRadius;
      pwm_duty[2] /= ((kRoot3/2.) + (1./2.) + (kWheelLength))/kWheelRadius;


      /* Apply Dead-band */
      if(pwm_duty[0] > 0) pwm_duty[0] += kDeadbandUpperPWM0A;
      if(pwm_duty[0] < 0) pwm_duty[0] -= kDeadbandLowerPWM0A;
      if(pwm_duty[1] > 0) pwm_duty[1] += kDeadbandUpperPWM1A;
      if(pwm_duty[1] < 0) pwm_duty[1] -= kDeadbandLowerPWM1A;
      if(pwm_duty[2] > 0) pwm_duty[2] += kDeadbandUpperPWM2A;
      if(pwm_duty[2] < 0) pwm_duty[2] -= kDeadbandLowerPWM2A;
 
      /* Move PWM duty center */
      pwm_duty[0] += 1500000; //ns
      pwm_duty[1] += 1500000; //ns
      pwm_duty[2] += 1500000; //ns

      printf("|-----------+-----------+-----------|\n");
      printf("|d0: %7d|d1: %7d|d2: %7d|\n", pwm_duty[0], pwm_duty[1], pwm_duty[2]);
      printf("+-----------+-----------+-----------+\n");

      /* Send command via Wi-Fi UDP socket */
      sprintf(command, "%d,%d,%d,%d,%d", pwm_duty[0], pwm_duty[1], pwm_duty[2], LL, RL);
      /* TODO: C. Send command string to UDP server using sendto() funtion */
      sendto(/* Code */);
    }
  }

done:
  sprintf(command, "%s", "exit");
  /* TODO: D. Send command string to UDP server using sendto() funtion */
  sendto(/* Code */);
  system("clear");
  printf("%s\n", command);
  return 0;
}
