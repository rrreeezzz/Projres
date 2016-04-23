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
#include <pthread.h>
#include "client_function.h"
#include "protocole.h"
#include "message.h"
#include "transfert.h"
#include "message.h"
#include "utilities.h"

pthread_t pid_transfer;

extern void ask_name();
extern void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients, waitList *waitlist);
extern void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients);
extern void handler_sigint();
extern int *init_server();
extern void routine_server(int *server_sockfd);
extern void cmde_host(fd_set *readfds, int *server_sockfd, int *maxfds, client_data *fd_array, int *num_clients, waitList *waitlist);
extern void slash_msg(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_all(int mod, char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern int is_sep(char c);
extern int my_count_word(const char *str);
extern int help(char * msg);

#endif
