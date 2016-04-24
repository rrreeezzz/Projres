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

int online;

extern int connect_serv();
extern int search_serv(char *buf, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist);

#endif
