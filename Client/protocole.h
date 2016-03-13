#ifndef __PROTOCOLE_H__
#define __PROTOCOLE_H__

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "message.h"
#include "client_function.h"
#include "transfert.h"

extern void send_msg(message *segment, int *fd);
extern int rcv_protocol_parser(char *msg, message *msg_rcv);
extern void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds);

#endif
