#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define WRITE_SIZE 80
#define MSG_SIZE WRITE_SIZE+25

extern void viderBuffer();
extern struct hostent * ask_server_adress(int *port);

#endif
