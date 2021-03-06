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
#include <string.h>
#include <ctype.h>
#include "client_function.h"
#include "protocole.h"
#include "message.h"
#include "transfert.h"
#include "message.h"
#include "utilities.h"

extern int userInterface_fd;

extern void ask_name();
extern void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients, waitList *waitlist);
extern void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients);
extern void handler_sigint();
extern int *init_server();
extern void * routine_ping(void *arg);
extern void routine_server(int *server_sockfd);
extern void cmde_host(int fd,fd_set *readfds, int *server_sockfd, int *maxfds, client_data *fd_array, int *num_clients, waitList *waitlist);
extern void slash_abort(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_vocal(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_transfer(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_pm(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_msg(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern void slash_all(int mod, char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients);
extern int is_sep(char c);
extern int my_count_word(const char *str);
extern void help(char * msg);
extern int protocol_parser(char *msg, message *msg_rcv);
extern int connect_serv();
extern int search_serv(char *buf, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist);
extern void sendUiMsg(char * content, fd_set *readfds, client_data *fd_array, int *num_clients);
extern int get_serv_status(client_data *fd_array, int *num_clients, fd_set *readfds);
extern int erase_serv(client_data *fd_array, int *num_clients, fd_set *readfds);

#endif
