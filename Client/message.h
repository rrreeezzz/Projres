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
#define MAX_SIZE_ADDRESS 22

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
  char address_client[INET_ADDRSTRLEN];
  int rdy;
  message *msg_rcv;
} client_data;

typedef struct {
  int nb_connect;
  int waiting[MAX_CLIENTS];
} waitList;

typedef struct {
  char username[MAX_SIZE_USERNAME];
  char address[MAX_SIZE_ADDRESS];
} annuaireData;

extern void session_initiate(message *segment);
extern void session_accept(message *segment);
extern void session_confirmed(message *segment);
extern void session_denied(message *segment, int type);
extern void normal_msg(message *segment, char * data);
extern void session_end(message *segment);
extern void session_aborted(message *segment);
void transfer_accept(message *segment, char *filename);
void transfer_refused(message *segment);
void transfer_initiate(message *segment, char *filename, int taille);
void transfer_msg(message *segment, char *data);
void transfer_end(message *segment, char *filename);

#endif
