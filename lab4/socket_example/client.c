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

  char message[256];

  /* check arguments */
  printf("[EE405 Stream Client Example]\n");

  if (argc != 3) {
    printf("Usage: %s <IP> <port>\n", argv[0]);
    return -1;
  }

  /* 1. socket(): create client socket using stream socket */
  client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    printf("client: can not open socket.\n");
    return -1;
  }
  printf("client: socket file descriptor: %d\n", client_socket);

  /* 2. connect() */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr_len = sizeof(server_addr);

  ret = connect(client_socket, (struct sockaddr*)&server_addr, server_addr_len);
  if (ret < 0) {
    printf("client: can not connect socket to server.\n");
    return -1;
  }

  getsockname(client_socket, (struct sockaddr*)&client_addr, &client_addr_len);
  printf("client: server address: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
  printf("client: client address: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
  printf("client: connected.\n");

  /* 3. read() & write(): echo client */
  while(true) {
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
    nbytes = write(client_socket, message, strlen(message));
    printf("client send(%d): %s\n", nbytes, message);

    nbytes = read(client_socket, message, 255);
    message[nbytes] = '\0';
    printf("client recv(%d): %s\n", nbytes, message);
  }
  
  /* close client socket file */
  close(client_socket);
    
  return 0;
}
