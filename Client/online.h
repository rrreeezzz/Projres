#ifndef __ONLINE_H__
#define __ONLINE_H__

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
#include <net/if.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>

#include "message.h"
#include "protocole.h"
#include "serv_function.h"

int online;
#define MAX_SIZE_PARAMETER 70

extern int get_serv_status(client_data *fd_array, int *num_clients, fd_set *readfds);
extern int connect_serv(client_data *fd_array, int *num_clients, fd_set *readfds);
extern int search_serv(char *buf, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist);
extern int erase_serv(client_data *fd_array, int *num_clients, fd_set *readfds);
extern int get_Config(char * conf);

#endif
