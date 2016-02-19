#ifndef __DB_H__
#define __DB_H__

#include <stdio.h>
#include <stdlib.h>
#include <my_global.h>
#include <mysql/mysql.h>
#include <string.h>

#define EXIST 1
#define NEXIST 0

extern void error_mysql();
extern void connect_mysql();
extern void exit_mysql();
extern int exist_user_mysql(char *name);
extern int id_user_mysql(char *name);
extern void add_user_mysql(char *name);
extern int last_id_mysql();

/*Variable globale pour la database*/
MYSQL *db;

#endif
