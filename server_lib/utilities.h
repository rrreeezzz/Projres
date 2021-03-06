#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#define MAX_SIZE_PARAMETER 70
#define WRITE_SIZE 1000
#define MSG_SIZE WRITE_SIZE+25

typedef struct {
  int code;
  int length;
  char *msg_content;
  time_t temps; //temps en secondes depuis le 1er janvier 1970
} message;

extern int server_sockfd; // Le fd du serveur en global

extern int get_Config(char * conf);
extern int protocol_parser(char msg[], message *msg_rcv);
extern void send_msg(message *segment, int client_sockfd);

#endif
