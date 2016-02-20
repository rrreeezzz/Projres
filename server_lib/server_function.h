#ifndef __SERVER_FUNCTION_H__
#define __SERVER_FUNCTION_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

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
  int id_client;
} client_data;

extern int rechercheCmd(const char *msg);
extern void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients);
extern void handler_sigint();
extern int * init_server();
extern void routine_server(int * server_sockfd);
extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern void login_client(int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);
extern int search_client(int fd, client_data *fd_array, int *num_clients);

#endif
