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

typedef struct {
  int code;
  int length;
  char *msg_content;
  time_t temps; //temps en secondes depuis le 1er janvier 1970
} message;

#endif
