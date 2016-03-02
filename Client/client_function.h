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

#define WRITE_SIZE 100
#define MSG_SIZE WRITE_SIZE+25
#define MAX_CLIENTS 95
#define Max(x,y) (x>y ? x:y)
#define QUIT 0
#define MSG 1
#define ALL1 2
#define ALL2 3
#define GRP 4
#define HELP 5

typedef struct {
  int fd_client;
  char id_client;
  char name_client[16];
} client_data;

char General_Name[16];

extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void login_client(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);
extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);
extern int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array);
extern int search_client_name(char *user, client_data *fd_array, int *num_clients);
extern int search_client_id(int fd, client_data *fd_array, int *num_clients);

#endif
