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
#include <time.h>
#include <arpa/inet.h>

#include "db.h"
#include "utilities.h"
#include "message.h"

#define INFO_SIZE 256

int server_sockfd;

extern void quit_server();
extern void handler_sigint();
extern void init_server();
extern void routine_server();
extern void host_cmde();
extern void traiter_requete(char *buffer, int client_sockfd);

#endif
