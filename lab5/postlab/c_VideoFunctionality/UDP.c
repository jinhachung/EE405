#include "UDP.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct UDP_socket
{
  int sock;
  struct sockaddr_in recv_addr;
  struct sockaddr_in send_addr;
  unsigned int len_send_addr;
};

struct UDP_socket *UDP_new_socket (void)
{
  return (struct UDP_socket *) malloc (sizeof (struct UDP_socket));
}

int UDP_socket_init (struct UDP_socket *udp)
{
  if (udp == NULL)
    return -1;

  udp->sock = socket (PF_INET, SOCK_DGRAM, 0);
  memset (&udp->recv_addr, 0, sizeof udp->recv_addr);
  memset (&udp->send_addr, 0, sizeof udp->send_addr);

  return 0;
}

int UDP_set_server (struct UDP_socket *udp, int port)
{
  if (udp == NULL)
    return -1;

  udp->recv_addr.sin_family = AF_INET;
  udp->recv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  udp->recv_addr.sin_port = htons (port);

  bind (udp->sock, (struct sockaddr *)&udp->recv_addr, sizeof udp->recv_addr);

  return 0;
}

int UDP_set_client (struct UDP_socket *udp, int port, char *str_ip)
{
  if (udp == NULL)
    return -1;
  
  udp->send_addr.sin_family = AF_INET;
  udp->send_addr.sin_addr.s_addr = inet_addr (str_ip);
  udp->send_addr.sin_port = htons (port);

  udp->len_send_addr = sizeof udp->send_addr;

  return 0;
}

int UDP_recv (struct UDP_socket *udp, void *buf, size_t size)
{
  return recvfrom (udp->sock, buf, size, MSG_DONTWAIT,
                   (struct sockaddr *)&udp->send_addr, &udp->len_send_addr);
}

int UDP_send (struct UDP_socket *udp, void *buf, size_t size)
{
  return sendto (udp->sock, buf, size, 0,
                 (struct sockaddr *)&udp->send_addr, udp->len_send_addr);
}

void UDP_close (struct UDP_socket *udp)
{
  close (udp->sock);
  free (udp);
}
