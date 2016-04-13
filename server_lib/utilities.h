#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "message.h"

#define MAX_SIZE_PARAMETER 70

extern int get_Config(char * conf);
extern int protocol_parser(char *msg, message *msg_rcv);

#endif
