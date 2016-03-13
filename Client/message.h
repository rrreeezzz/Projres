#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define WRITE_SIZE 100
#define MSG_SIZE WRITE_SIZE+25
#define MAX_CLIENTS 95
#define Max(x,y) (x>y ? x:y) //pour optdesc
#define MAX_SIZE_USERNAME 16

char General_Name[MAX_SIZE_USERNAME];

typedef struct {
  int code;
  int length;
  char *msg_content;
  time_t temps; //temps en secondes depuis le 1er janvier 1970
} message;

typedef struct {
  int fd_client;
  char id_client;
  char name_client[MAX_SIZE_USERNAME];
  int rdy;
} client_data;

extern void session_initiate(message *segment);
extern void session_accept(message *segment);
extern void session_confirmed(message *segment);
extern void session_denied(message *segment, int type);
extern void normal_msg(message *segment, char * data);
extern void session_end(message *segment);

#endif
