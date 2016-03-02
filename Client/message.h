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
#define Max(x,y) (x>y ? x:y)
#define QUIT 0
#define MSG 1
#define ALL1 2
#define ALL2 3
#define GRP 4
#define HELP 5

typedef struct {
  int code;
  int length;
  char *msg_content;
  time_t temps; //temps en secondes depuis le 1er janvier 1970
} message;

extern void send_time(time_t *temps);

#endif
