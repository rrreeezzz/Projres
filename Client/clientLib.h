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

typedef struct {
  int fd_client;
  char id_client;
  char name_client[16];
} client_data;

char General_Name[16];

//Fonction que le fichier va chercher a coté, pour pas a avoir a iclure client_function.h dans ce fichier
extern char General_Name[16];
extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void login_client(int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);

extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);
extern int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array);

#endif
