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

#define WRITE_SIZE 100
#define MSG_SIZE WRITE_SIZE+25

typedef struct {
  int fd_client;
  char id_client;
  char name_client[16];
} client_data;

typedef struct {
  int code;
  char *msg_content;
  //time
} message;

char General_Name[16];

//Fonction que le fichier va chercher a coté, pour pas a avoir a iclure client_function.h dans ce fichier
extern char General_Name[16];
extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void login_client(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);
extern void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);

extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);
extern int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array);

#endif
