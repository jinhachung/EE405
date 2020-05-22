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
  
  int server_socket;
  int client_socket;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int server_addr_len;
  int client_addr_len;

  int option;
  char message[256];

  printf("[EE405 Stream Server Example]\n");

  /* check arguments */
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return -1;
  }

  /* 1. socket(): create server socket using stream socket */
  server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) {
    printf("server: can not open socket.\n");
    return -1;
  }
  printf("server: socket file descriptor: %d\n", server_socket);
  option = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  /* 2. bind() */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));
  server_addr_len = sizeof(server_addr);

  ret = bind(server_socket, (struct sockaddr*)&server_addr, server_addr_len);
  if (ret < 0) {
    printf("server: can not bind socket to local IP address.\n");
    return -1;
  }

  /* 3. listen() */
  ret = listen(server_socket, 10);
  if (ret < 0) {
    printf("server: can not listen connect.\n");
    return -1;
  }

  /* 4. accept() */
  memset(&client_addr, 0, sizeof(client_addr));
  printf("server: server address: %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
  printf("server: waiting connection request.\n");

  while(true) {
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    /* blocked here until connection request is received */
    if (client_socket < 0) {
      printf("server: accept failed.\n");
      continue;
    }

    printf("server: client address: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
 
    /* 5. read() & write(): echo server */
    while((nbytes = read(client_socket, message, 255)) != 0) {
      if (nbytes < 0) {
        printf("server recv: failed\n");
        break;
      }
      if (nbytes == 0) {
        printf("server recv: EOF\n");
        break;
      }
      message[nbytes] = '\0';
      printf("server recv(%d): %s\n", nbytes, message);
      nbytes = write(client_socket, message, strlen(message));
      printf("server send(%d): %s\n", nbytes, message); 
    }
    
    /* close client socket file */
    close(client_socket);
  }

    

  /* close server socket file */
  close(server_socket);

  return 0;
}
