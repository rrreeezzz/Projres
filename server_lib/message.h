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

#include "db.h"
#include "utilities.h"

extern void im_on(message *segment);
extern void already_exist(message *segment);
extern void send_ip(message *rcv, message *segment);
extern void no_exist(message *segment);

#endif
