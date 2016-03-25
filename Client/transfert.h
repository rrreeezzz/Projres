#ifndef __TRANSFERT_H__
#define __TRANSFERT_H__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "protocole.h"
#include "message.h"

typedef struct paramThread{
  int file_fd;
  int client_sockfd;
  char filename[256];
  fd_set *readfds;
  client_data *fd_array;
  int *num_clients;
} paramThread;

extern void init_transfer(int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void parser_transfer(int mod, char *msg, char *user, char *filename, int *taille);
extern int ask_transfer(message *msg, char *filename);
extern void prepare_transfer(message *msg, int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void *file_transfer(void *arg);

#endif
