#ifndef __CLIENT_H__
#define __CLIENT_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include "protocole.h"
#include "message.h"
#include "serv_function.h"
#include "annuaire.h"
#include "utilities.h"

extern int userInterface_fd;

extern void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds);
extern int login_client(message *msg_send, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist);
extern void viderBuffer();
extern struct hostent * ask_server_address(int *port, annuaireData *user);
extern int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, annuaireData *user, waitList *waitlist);
extern int connect_refuse(client_data *fd_array, int *num_clients, fd_set *readfds, char *msg, waitList *waitlist);
extern int connect_accept(client_data *fd_array, int *num_clients, fd_set *readfds, char *msg, waitList *waitlist);
extern int disconnect(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg);
extern int search_client_id_by_name(char *user, client_data *fd_array, int *num_clients);
extern int search_client_id_by_fd(int fd, client_data *fd_array, int *num_clients);
extern int search_client_array_by_fd(int fd, client_data *fd_array, int *num_clients);
extern int search_client_array_by_name(char *user, client_data *fd_array, int *num_clients);
extern void client_ready(int fd, client_data *fd_array, int *num_clients);
extern int search_client_ready_by_fd(int fd, client_data *fd_array, int *num_clients);
extern int search_client_fd_by_name(char *user, client_data *fd_array, int *num_clients);
extern char * search_client_address_by_name(char *user, client_data *fd_array, int *num_clients);
extern int search_client_waiting_array_by_fd(int fd, client_data *fd_array, int *num_clients, waitList waitlist);
extern int search_client_waiting_fd_by_name(char *user, client_data *fd_array, int *num_clients, waitList waitlist);
extern int search_waiting_array_by_fd(int fd, waitList waitlist);

#endif
#endif
