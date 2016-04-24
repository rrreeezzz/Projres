#include "db.h"

void error_mysql(){
  fprintf(stderr, "%s\n", mysql_error(db));
  mysql_close(db);
  exit(EXIT_FAILURE);
}

void connect_mysql(){ //si ne se connecte pas, c'est ce fichier qui gère l'erreur donc pas besoin de renvoyer qqchose

/*On récupère les options.*/
char admin[MAX_SIZE_PARAMETER] = "user";
if(get_Config(admin) < 0){
  perror("Erreur recupération user database.");
  exit(EXIT_FAILURE);
}

admin[strlen(admin) -1] = '\0';

char pswd[MAX_SIZE_PARAMETER] = "password";
if(get_Config(pswd) < 0){
  perror("Erreur recupération password database.");
  exit(EXIT_FAILURE);
}

pswd[strlen(pswd) -1] = '\0';

db = mysql_init(NULL);

if (db == NULL){
  fprintf(stderr, "%s\n", mysql_error(db));
  exit(EXIT_FAILURE);
}

if (mysql_real_connect(db, "localhost", admin, pswd, NULL, 0, NULL, 0) == NULL) //pour l'instant en clair, mais créer fichier de configuration
error_mysql(db);

if (mysql_query(db, "USE pswd")) //retourne 0 si succes, on utilise la database chat existe
error_mysql();

char name[5];
strcpy(name, "admin");
if(!exist_user_mysql(name)){
  char operation[REQUEST_SIZE]; //mettre un define
  sprintf(operation, "INSERT INTO user VALUES(0, 'admin', '0.0.0.0')"); //pour eviter bug si y'a personne, changer en "you" ou un truc du genre ?

  if (mysql_query(db, operation))
  error_mysql();
}
}

void exit_mysql(){
  mysql_close(db);
}

int add_user_mysql(char *data){

  char operation[REQUEST_SIZE]; //mettre un define
  char name[MAX_SIZE_PARAMETER];
  char ip[MAX_SIZE_PARAMETER];

  sscanf(data, "FROM:%s IP:%s", name, ip);

  printf("\e[1;33m[SERVER]\e[0m Add : \nName : %s, ip : %s\n", name, ip);

  if(exist_user_mysql(name)){

    sprintf(operation, "UPDATE user SET IP = '%s' WHERE NAME = '%s';", ip, name);

    if (mysql_query(db, operation))
    error_mysql();

  }else{

    sprintf(operation, "INSERT INTO user VALUES(%d, '%s', '%s')", last_id_mysql() + 1, name, ip);

    if (mysql_query(db, operation))
    error_mysql();

  }
  return 0;

}

int exist_user_mysql(char *name){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[REQUEST_SIZE]; //mettre un define
  int nb;

  if(NULL != strchr(name, '\''))
    return NEXIST;

  sprintf(operation, "SELECT COUNT(*) FROM user WHERE NAME = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
  error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  nb = atoi(row[0]);
  mysql_free_result(result);

  if(nb)
  return EXIST;
  else
  return NEXIST;

}

int id_user_mysql(char *name){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[REQUEST_SIZE]; //mettre un define
  int id;

  sprintf(operation, "SELECT ID FROM user WHERE NAME = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
  error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  id = atoi(row[0]);
  mysql_free_result(result);

  return id; //ID de l'USER

}

void ip_user_mysql(char *name, char *ip){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[REQUEST_SIZE]; //mettre un define

  sprintf(operation, "SELECT IP FROM user WHERE NAME = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
  error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  strcpy(ip, row[0]);
  mysql_free_result(result);

}

int last_id_mysql(){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[REQUEST_SIZE]; //mettre un define
  int id;

  sprintf(operation, "SELECT MAX(ID) FROM user"); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
  error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  id = atoi(row[0]);
  mysql_free_result(result);

  return id; //ID de l'USER
}

char * name_user_mysql(int id){

  if(id > last_id_mysql() || id < 1){
    printf("Error with client id\n");
    exit(EXIT_FAILURE);
  }

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[REQUEST_SIZE]; //mettre un define

  char *name = (char *) malloc(15*sizeof(char)); //mettre un define, sachant que pas plus grand que 15 dans login_client, va falloir faire un free
  sprintf(operation, "SELECT NAME FROM user WHERE ID = '%d'", id); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
  error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  sprintf(name, "%s", row[0]);
  name[strlen(name)] = '\0';
  mysql_free_result(result);

  return name;
}

struct tm * time_server(){

  time_t secondes;
  struct tm * instant;

  instant = (struct tm *) malloc(sizeof(struct tm));

  time(&secondes);
  instant=localtime(&secondes);

  printf("%d/%d/%d ; %d:%d:%d\n", instant->tm_mday+1, instant->tm_mon+1,instant->tm_year+1900, instant->tm_hour, instant->tm_min, instant->tm_sec);
  return instant;
}

void last_connection_mysql(int id){

  char operation[REQUEST_SIZE]; //mettre un define

  sprintf(operation, "UPDATE user SET Last_Connection = NOW() WHERE Id = %d", id); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if (mysql_query(db, operation))
  error_mysql();

}

int del_user_mysql(char *data){

  char operation[REQUEST_SIZE]; //mettre un define
  char name[MAX_SIZE_PARAMETER];

  sscanf(data, "FROM:%s", name);

  if(!exist_user_mysql(name))
  return -1;

  sprintf(operation, "DELETE FROM user WHERE NAME='%s'", name);

  if (mysql_query(db, operation))
  error_mysql();

  printf("\e[1;33m[SERVER]\e[0m %s s'est effacé de la base de donnée.\n", name);

  return 0;

}
