#ifndef __MINIMALUI_H__
#define __MINIMALUI_H__

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MINIMALUI_NOSTDIN 0
#define MINIMALUI_STDIN 1

char General_Name[MAX_SIZE_USERNAME];
char adresseClientPrincipal[16];
int portClientPrincipal;
int fdClientPrincipal;
fd_set readfds;
int contact_list_position;
int nb_elems_contact_list;
int connected_list_position;
int nb_elems_connected_list;

typedef struct {
  int code;
  int length;
  char *msg_content;
  time_t temps; //temps en secondes depuis le 1er janvier 1970
} message;

#include "applicationProtocol.h"
#include "message.h"
#include "protocol.h"
#include "basicCommunication.h"

#endif
