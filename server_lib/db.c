#include "db.h"

void error_mysql(){
  fprintf(stderr, "%s\n", mysql_error(db));
  mysql_close(db);
  exit(EXIT_FAILURE);
}

void connect_mysql(){ //si ne se connecte pas, c'est ce fichier qui gère l'erreur donc pas besoin de renvoyer qqchose

  db = mysql_init(NULL);

  if (db == NULL){
    fprintf(stderr, "%s\n", mysql_error(db));
    exit(EXIT_FAILURE);
  }

  if (mysql_real_connect(db, "localhost", "root", "test49", NULL, 0, NULL, 0) == NULL) //pour l'instant en clair, mais créer fichier de configuration
  error_mysql(db);

  if (mysql_query(db, "USE pswd")) //retourne 0 si succes, on utilise la database pswd existe
  error_mysql();
}

void exit_mysql(){
  mysql_close(db);
}

void add_user_mysql(char *name){

  char operation[256]; //mettre un define

  sprintf(operation, "INSERT INTO pswd VALUES(%d, '%s')", last_id_mysql() + 1, name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if (mysql_query(db, operation))
    error_mysql();

}

int exist_user_mysql(char *name){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[256]; //mettre un define
  int nb;

  sprintf(operation, "SELECT COUNT(*) FROM pswd WHERE USER = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

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
  char operation[256]; //mettre un define
  int id;

  sprintf(operation, "SELECT ID FROM pswd WHERE USER = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
    error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  id = atoi(row[0]);
  mysql_free_result(result);

  return id; //ID de l'USER

}

int last_id_mysql(){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[256]; //mettre un define
  int id;

  sprintf(operation, "SELECT MAX(ID) FROM pswd"); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if(mysql_query(db, operation))
    error_mysql();

  result = mysql_use_result(db); //stock le resultat de mysql_query
  row =  mysql_fetch_row(result); //on recup le resultat du champ

  id = atoi(row[0]);
  mysql_free_result(result);

  return id; //ID de l'USER
}
