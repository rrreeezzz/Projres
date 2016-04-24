#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define WRITE_SIZE 1000
#define MSG_SIZE WRITE_SIZE+25
#define MAX_SIZE_PORT 5
#define MIN_SIZE_USERNAME 4
#define MAX_SIZE_USERNAME 16
#define MAX_SIZE_ADDRESS 22

char General_Name[MAX_SIZE_USERNAME];
char adresseClientPrincipal[16];
int portClientPrincipal;
int fdClientPrincipal;

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
