#ifndef __ANNUAIRE_H__
#define __ANNUAIRE_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client_function.h"
#include "message.h"

#define CONTACT "contact.txt"

extern int update_contact();
extern off_t search_contact(char *name, int contact_list);
extern int open_directory();
extern void ask_contact_name(char *username);
extern int change_contact_data(int original, off_t delete_line);
extern void ask_contact_adress(char * temp);
extern int connect_to_contact(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg);

#endif
