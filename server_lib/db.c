#include "db.h"

void error_mysql(){
  fprintf(stderr, "%s\n", mysql_error(db));
  mysql_close(db);
  exit(EXIT_FAILURE);
}

void connect_mysql(){ //si ne se connecte pas, c'est ce fichier qui gère l'erreur donc pas besoin de renvoyer qqchose

  FILE * file; //pas posix mais serveur que sur linux ??
  char admin[40];//metre un define
  char pswd[40];
  char line[256];

  if((file = fopen("../conf.txt", "r")) == NULL){
    perror("Error with oppening configuration file");
    exit(EXIT_FAILURE);
  }

  /*A changer car très incertain, existe des librairies de fichier de conf*/
  fgets(line, sizeof(line), file); //lecture premiere ligne
  memset (line, '\0', sizeof(line));//réinitialisation chaine
  fgets(line, sizeof(line), file);
  sscanf(line, "user:%s", admin);
  memset (line, '\0', sizeof(line));//réinitialisation chaine
  fgets(line, sizeof(line), file);
  sscanf(line, "password:%s", pswd);

  db = mysql_init(NULL);

  if (db == NULL){
    fprintf(stderr, "%s\n", mysql_error(db));
    exit(EXIT_FAILURE);
  }

  if (mysql_real_connect(db, "localhost", admin, pswd, NULL, 0, NULL, 0) == NULL) //pour l'instant en clair, mais créer fichier de configuration
  error_mysql(db);

  if (mysql_query(db, "USE chat")) //retourne 0 si succes, on utilise la database chat existe
  error_mysql();

  fclose(file);
}

void exit_mysql(){
  mysql_close(db);
}

void add_user_mysql(char *name, char *pswd){

  char operation[256]; //mettre un define

  sprintf(operation, "INSERT INTO users VALUES(%d, '%s', NOW(), NOW(), '%s')", last_id_mysql() + 1, name, pswd);

  if (mysql_query(db, operation))
    error_mysql();

}

int exist_user_mysql(char *name){

  MYSQL_RES *result = NULL; //This structure represents the result of a query that returns rows
  MYSQL_ROW row = NULL; //This is a type-safe representation of one row of data
  char operation[256]; //mettre un define
  int nb;

  sprintf(operation, "SELECT COUNT(*) FROM users WHERE Name = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

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

  sprintf(operation, "SELECT Id FROM users WHERE Name = '%s'", name); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

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

  sprintf(operation, "SELECT MAX(Id) FROM users"); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

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
  char operation[256]; //mettre un define

  char *name = (char *) malloc(15*sizeof(char)); //mettre un define, sachant que pas plus grand que 15 dans login_client, va falloir faire un free
  sprintf(operation, "SELECT Name FROM users WHERE Id = '%d'", id); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

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

  char operation[256]; //mettre un define

  sprintf(operation, "UPDATE users SET Last_Connection = NOW() WHERE Id = %d", id); //cette commande ne donne qu'une colonne, avec un champ 1 si trouvé 0 sinon

  if (mysql_query(db, operation))
    error_mysql();

}