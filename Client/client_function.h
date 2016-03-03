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
#include <time.h>
#include "protocole.h"
#include "message.h"

extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void login_client(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);
extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);
extern int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array);
extern int search_client_name(char *user, client_data *fd_array, int *num_clients);
extern int search_client_id(int fd, client_data *fd_array, int *num_clients);

#endif
