#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern char General_Name[16];
extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);
extern int client(int *maxfds, fd_set *readfds);
extern int client(int *maxfds, fd_set *readfds);

#endif
