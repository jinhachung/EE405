#ifndef UDP_H
#define UDP_H

#include <sys/types.h>

struct UDP_socket;

struct UDP_socket *UDP_new_socket (void);
int UDP_socket_init (struct UDP_socket *udp);
int UDP_set_server (struct UDP_socket *udp, int port);
int UDP_set_client (struct UDP_socket *udp, int port, char *str_ip);
int UDP_recv (struct UDP_socket *udp, void *buf, size_t size);
int UDP_send (struct UDP_socket *udp, void *buf, size_t size);
void UDP_close (struct UDP_socket *udp);

#endif
