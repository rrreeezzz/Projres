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
#include <ctype.h>
#include "client_function.h"
#include "message.h"
#include "utilities.h"

#define CONTACT "contact.txt"

extern int add_contact(client_data *fd_array, int *num_clients, char * msg);
extern int add_contact_online(client_data *fd_array, annuaireData * contact, int *num_clients, char * msg);
extern int remove_contact (fd_set *readfds,client_data *fd_array, int *num_clients,char *msg);
extern int print_contact_list (fd_set *readfds,client_data *fd_array, int *num_clients);
extern int print_connected_user(fd_set *readfds,client_data *fd_array, int *num_clients);
extern off_t search_contact(char *name, int contact_list);
extern int open_directory();
extern void ask_contact_name(char *username);
extern int remove_contact_data(int original, off_t delete_line);
extern void ask_contact_address(char * temp);
extern int connect_to_contact(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg, waitList *waitlist);

#endif
