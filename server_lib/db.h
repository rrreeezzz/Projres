#ifndef __DB_H__
#define __DB_H__

#include <stdio.h>
#include <stdlib.h>
#include <my_global.h>
#include <mysql/mysql.h>
#include <string.h>
#include <time.h>

#include "utilities.h"

#define EXIST 1
#define NEXIST 0
#define REQUEST_SIZE 256

extern void error_mysql();
extern void connect_mysql();
extern void exit_mysql();
extern int exist_user_mysql(char *name);
extern int id_user_mysql(char *name);
extern char * name_user_mysql(int id);
extern int add_user_mysql(char *data);
extern void ip_user_mysql(char *name, char *ip);
extern int last_id_mysql();
extern struct tm * time_server();
extern int del_user_mysql(char *data);

/*Variable globale pour la database*/
MYSQL *db;

#endif
