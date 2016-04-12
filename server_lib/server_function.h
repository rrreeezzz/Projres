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

#include "db.h"
#include "utilities.h"
#include "message.h"

#define INFO_SIZE 256

extern void quit_server(int server_sockfd);
extern void handler_sigint();
extern int init_server();
extern void routine_server(int server_sockfd);
extern void host_cmde(int server_sockfd);
extern void traiter_requete(int server_sockfd, char *buffer, struct sockaddr remote_addr);

#endif
