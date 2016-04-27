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
#include "utilities.h"

#define MAX_SIZE_FILENAME 256
#define MAX_SIZE_DATA 50

typedef struct paramThread{
  int file_fd;
  int client_sockfd;
  char filename[MAX_SIZE_FILENAME];
  int filesize;
  fd_set *readfds;
  client_data *fd_array;
  int *num_clients;
} paramThread;

extern void init_transfer(char *filepath, int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void parser_transfer(int mod, char *msg, char *user, char *filename, int *taille);
extern int ask_transfer(message *msg, char *filename);
extern void prepare_transfer(message *msg, int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void *file_transfer(void *arg);
extern void init_vocal(int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void prepare_vocal(int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void *vocal_transfer(void *arg);

#endif
