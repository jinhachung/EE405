#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
  int ret;
  int nbytes;

  int client_socket;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int server_addr_len;
  int client_addr_len;

  int option;
  char message[256];

  printf("[EE405 Datagram Client Example]\n"); 

  /* check arguments */
  if (argc != 3) {
    printf("Usage: %s <IP> <port>\n", argv[0]);
    return -1;
  }

  /* 1. socket(): create servoer socket using datagram socket */
  client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (client_socket < 0) {
    printf("client: can not open socket.\n");
    return -1;
  }
  printf("client: socket file descriptor: %d\n", client_socket);

  // /* 2. bind() */
  // memset(&client_addr, 0, sizeof(client_addr));
  // client_addr.sin_family = AF_INET;
  // client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // client_addr.sin_port = htons(atoi(argv[1]));
  // client_addr_len = sizeof(client_addr);

  // ret = bind(client_socket, (struct sockaddr*)&client_addr, client_addr_len);
  // if (ret < 0) {
  //   printf("client: can not bind socket to local IP address.\n");
  //   return -1;
  // }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr_len = sizeof(server_addr);

  /* 3. recvfrom() & sendto(): echo client */
  while(true) {
    printf("client: waiting user input.\n");
    nbytes = read(0, message, 255);
    if (nbytes < 0) {
      printf("client read: failed");
      break;
    }
    if (nbytes == 0) {
      printf("client read: EOF\n");
      break;
    }
    message[nbytes] = '\0';
    nbytes = sendto(client_socket, message, strlen(message), 0, (struct sockaddr*)&server_addr, server_addr_len);
    printf("client send(%d): %s\n", nbytes, message);

    nbytes = recvfrom(client_socket, message, 255, 0, (struct sockaddr*)&server_addr, &server_addr_len);
    message[nbytes] = '\0';
    printf("client recv(%d): %s\n", nbytes, message);
  }

  /* close server socket file */
  close(client_socket);

  return 0;
}