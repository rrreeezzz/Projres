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

extern void init_transfer(int client_sockfd);
extern int ask_transfer(message *msg, char *filename);
extern void prepare_transfer(message *msg, int client_sockfd);
extern void *file_transfer(void *arg);

#endif
