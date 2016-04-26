#include "annuaire.h"

int add_contact (fd_set *readfds, client_data *fd_array, int *num_clients, char * msg) {

  /*Permet d'ajouter un contact ou de mettre la base de données a jour.*/

  if(strlen(msg) < MIN_SIZE_CONTACT || strlen(msg) > MAX_SIZE_CONTACT) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/add username address"RED":"BLUE"port\".\n[PROGRAM] Username must be between 4 and 16 characters."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }

  off_t offset;
  int contact_file;
  int new_file = -1;
  int port = -1;
	int i = 0;
  int countpoint, countspace, countcolon;
  int bchar;
  char *pch;
  char *posSpace = NULL;
  char *posPort = NULL;
  char *contact_addr = NULL;
  char contact_data[3][strlen(msg)]; // 3 au cas où le client entre ""/add username address port"
  char data[MAX_SIZE_CONTACT];

  memset(contact_data[0], '\0', strlen(contact_data[0]));
  memset(contact_data[1], '\0', strlen(contact_data[1]));
  memset(contact_data[2], '\0', strlen(contact_data[2]));

  /* Init structure user */
  if((posSpace = strchr(msg, ' ')) == NULL) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/add username address"RED":"BLUE"port\".\n[PROGRAM] Username must be between 4 and 16 characters."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
	strcpy(data, posSpace+1);
	data[strlen(data) - 1] = '\0';
  /* Remplissage de la structure contact_data */
  pch = strtok (data," ");
  while (pch != NULL) {
    strcpy(contact_data[i], pch);
    pch = strtok (NULL, " ");
    i++;
  }
  /* Vérification de la longueur des données */
  if(strlen(contact_data[0]) < MIN_SIZE_USERNAME || strlen (contact_data[0]) > MAX_SIZE_USERNAME || strlen(contact_data[1])+strlen(contact_data[2]) < MIN_SIZE_ADDRESS-1 || strlen(contact_data[1])+strlen(contact_data[2]) > MAX_SIZE_ADDRESS-1) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/add username address"RED":"BLUE"port\" or \"/add username address port\".\n[PROGRAM] Username must be between 4 and 16 characters."RESET"\n\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  /* Vérification des caractères du username */
  for(i=0 ; i < strlen(contact_data[0]) ; i++) {
    bchar = (isalnum((unsigned char) contact_data[0][i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : Username must be composed of alphanumeric characters.\n[PROGRAM] Please use \"/add username address"RED":"BLUE"port\".\n"RESET"\n\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  /* Vérification du port */
  /* S'il y a un port rentré en contact_data[2]: concaténer contact_data[1] si le port est correct.
     Sinon, on vérifie le port en séparant contact_data[1] */
  if(contact_data[2][0] != '\0') {
    port = atoi(contact_data[2]);
    if(port <= 0 || port >= 65536 || ((strcmp(contact_data[1], "0.0.0.0") == 0) && (port == General_Port))) {
      port = -1;
    } else {
      strcat(contact_data[1], ":");
      strcat(contact_data[1], contact_data[2]);
    }
  } else {
    if ((posPort = strchr(contact_data[1], ':')) != NULL) {
      port = (int) strtol(posPort+1, NULL, 10);
      if(port <= 0 || port >= 65536 || ((strcmp(contact_data[1], "0.0.0.0") == 0) && port == General_Port))
        port = -1;
    }
  }
  if (port == -1) {
    printf(BLUE"[PROGRAM] Error command : Wrong port. Please use \"/add username address"RED":"BLUE"port\".\n[PROGRAM] Username must be between 4 and 16 characters."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  /* Vérification des caractères de l'adresse */
  bchar = 0; countpoint = 0; countcolon = 0;
  for(i=0 ; i < strlen(contact_data[1]) ; i++) {
    if(contact_data[1][i] == '.') {countpoint++;}
    if(contact_data[1][i] == ':') {countcolon++;}
    bchar = (isdigit((unsigned char) contact_data[1][i]) || contact_data[1][i] == '.' || contact_data[1][i] == ':' ) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar || countpoint != 3 || countcolon != 1) {
    printf(BLUE"[PROGRAM] Error command : Address must be composed of digit characters. Separator must be a space ' ' or a colon ':'.\n[PROGRAM] Please use \"/add username address"RED":"BLUE"port\"."RESET"\n\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }

  /* Ajout des données dans la structure */
  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
  sprintf(user->username, "%s", contact_data[0]);
  sprintf(user->address, "%s", contact_data[1]);
  /* Fin de l'initialisation / gestion des erreurs */

  /* On cherche si l'utilisateur est en ligne, sinon on vérifie s'il existe dans le fichier. */
  contact_addr = search_client_address_by_name(user->username, fd_array, num_clients);
  if (contact_addr != NULL) {
    if (add_contact_online(fd_array, user, num_clients, msg) != 0) {
      printf(BLUE"[PROGRAM] Error while adding online contact."RESET"\n");
      //on avertis l'ui si elle est connectee
      if (userInterface_fd > 0) {
        sendUiMsg("ADDCONTACTERROR Error while adding online contact.\n",readfds,fd_array,num_clients);
      }
      free(user);
      return -1;
    }
    free(user);
    return 0;
  } else { // Contact n'est pas en ligne
    if ((contact_file = open_directory()) == -1 ) {
      //on avertis l'ui si elle est connectee
      if (userInterface_fd > 0) {
        sendUiMsg("ADDCONTACTERROR Error opening contact file.\n",readfds,fd_array,num_clients);
      }
      free(user);
      return -1;
    }
    /* On cherche si le contact existe dans le fichier, si ce n'est pas le cas on vérifie si le fichier est vide ou pas, puis on le créé, sinon on le modifie. */
    if ((offset = search_contact(user->username, contact_file)) == -1) {
      if(lseek(contact_file, 0, SEEK_END) < 0) {
        perror(BLUE"[PROGRAM] Error while seeking end of contact file : file does not exist ?"RESET);
        //on avertis l'ui si elle est connectee
        if (userInterface_fd > 0) {
          sendUiMsg("ADDCONTACTERROR Error seeking end of contact file : file does not exist ?\n",readfds,fd_array,num_clients);
        }
        free(user);
        close(contact_file);
        close(new_file);
        return -1;
      }
      if(write(contact_file, user, sizeof(annuaireData)) < 0) {
        perror(BLUE"[PROGRAM] Error while writing contact update "RESET);
        //on avertis l'ui si elle est connectee
        if (userInterface_fd > 0) {
          sendUiMsg("ADDCONTACTERROR Error writing contact update.\n",readfds,fd_array,num_clients);
        }
        free(user);
        close(contact_file);
        close(new_file);
        return -1;
      }
      printf(BLUE"[PROGRAM] Contact "RED"%s"BLUE" added !"RESET"\n", user->username);
      //on avertis l'ui si elle est connectee
      if (userInterface_fd > 0) {
        char content[MSG_SIZE];
        sprintf(content,"ADDCONTACTCONFIRM %s\n",user->username);
        sendUiMsg(content,readfds,fd_array,num_clients);
      }
    } else {
      /* On supprime les anciennes données puis on réécrit les nouvelles */
      new_file = remove_contact_data(contact_file, offset);
      if(lseek(new_file, 0, SEEK_END) < 0) {
        perror(BLUE"[PROGRAM] Error while seeking end of new contact file : file does not exist ?"RESET);
        //on avertis l'ui si elle est connectee
        if (userInterface_fd > 0) {
          sendUiMsg("ADDCONTACTERROR Error seeking end of new contact file : file does not exist ?\n",readfds,fd_array,num_clients);
        }
        free(user);
        close(contact_file);
        close(new_file);
        return -1;
      }
      if(write(new_file, user, sizeof(annuaireData)) < 0) {
        perror(BLUE"[PROGRAM] Error while writing contact update "RESET);
        //on avertis l'ui si elle est connectee
        if (userInterface_fd > 0) {
          sendUiMsg("ADDCONTACTERROR Error writing contact update.\n",readfds,fd_array,num_clients);
        }
        free(user);
        close(contact_file);
        close(new_file);
        return -1;
      }
      printf(BLUE"[PROGRAM] Contact "RED"%s"BLUE" updated !"RESET"\n", user->username);

      //on avertis l'ui si elle est connectee
      if (userInterface_fd > 0) {
        char content[MSG_SIZE];
        sprintf(content,"UPDATECONTACTCONFIRM %s\n",user->username);
        sendUiMsg(content,readfds,fd_array,num_clients);
      }
    }

    free(user);
    close(contact_file);
    close(new_file);
    return 0;
  }
}

int remove_contact (fd_set *readfds,client_data *fd_array, int *num_clients, char *msg) {

  /* Vérification de la longueur du message reçu -> inutile de déclarer et allouer avant ça. */
  if(strlen(msg) < MIN_SIZE_USERNAME+8 || strlen(msg) > MAX_SIZE_USERNAME+8) { // "/remove " = 8
    printf(BLUE"[PROGRAM] Command too short or too long, please use \"/remove username\"."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("REMOVECONTACTERROR Command too long.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  /* Déclaration des variables */
  off_t offset;
  int contact_file;
  char *posSpace = NULL;
  char name[MAX_SIZE_USERNAME];
  int i;
  int bchar;

  /* Init structure user */
  if((posSpace = strchr(msg, ' ')) == NULL) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/remove username\".\n[PROGRAM] Username must be between 4 and 16 characters."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("REMOVECONTACTERROR Command error.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  strcpy(name, posSpace+1);
  name[strlen(name) - 1] = '\0';

  /* Vérification de la syntaxe de la commande */
  for(i=0 ; i < strlen(name) ; i++) {
    bchar = (isalnum((unsigned char) name[i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : Username must be composed of alphanumeric characters.\n[PROGRAM] Please use \"/add username address"RED":"BLUE"port\".\n"RESET"\n\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Wrong command.\n",readfds,fd_array,num_clients);
    }
    return -1;
  }
  /* Remplissage de la structure user */
  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
	strcpy(user->username, name);
  /* Fin de l'initialisation / gestion des erreurs */

  if ((contact_file = open_directory()) == -1 ) {
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Error opening contact file.\n",readfds,fd_array,num_clients);
    }
    free(user);
    return -1;
  }
  if ((offset = search_contact(user->username, contact_file)) == -1) {
    printf(BLUE"[PROGRAM] Error : contact was not found in contact list"RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Error : contact not found.\n",readfds,fd_array,num_clients);
    }
    free(user);
    close(contact_file);
    return -1;
  }
  if (remove_contact_data(contact_file, offset) == -1) {
    printf(BLUE"[PROGRAM] Error while removing contact's data"RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("ADDCONTACTERROR Error removing contact's data.\n",readfds,fd_array,num_clients);
    }
    free(user);
    close(contact_file);
    return -1;
  }

  //on avertis l'ui si elle est connectee
  if (userInterface_fd > 0) {
    char content[MSG_SIZE];
    sprintf(content,"REMOVECONTACTCONFIRM %s\n",user->username);
    sendUiMsg(content,readfds,fd_array,num_clients);
  }

  printf(BLUE"[PROGRAM] Contact "RED"%s"BLUE" removed !"RESET"\n", user->username);

  free(user);
  close(contact_file);
  return 0;
}

int print_contact_list (fd_set *readfds,client_data *fd_array, int *num_clients) {

  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
  int contact_file;
  char buf[sizeof(user)];

  if ((contact_file = open_directory()) == -1 ) {
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("CONTACTLIST Error opening contact file.\n",readfds,fd_array,num_clients);
    }
    free(user);
    return -1;
  }

  //on envoie la tete de la commande si l'ui est connectee
  if (userInterface_fd > 0) {
    int nbContacts = 0;
    char content[MSG_SIZE];

    //On compte le nombre de contacts en parcourant une 1ere fois
    while (read(contact_file, user, sizeof(annuaireData)) > 0) {
      if(user->username[0] != '\n') {
        nbContacts += 1 ;
      }
    }
    //On envoie le nombre de contacts
    sprintf(content,"CONTACTLIST %d\n",nbContacts);
    sendUiMsg(content,readfds,fd_array,num_clients);
    //On reset le fichier
    close(contact_file);
    contact_file = open_directory();
  }

  while (read(contact_file, user, sizeof(annuaireData)) > 0) {
    if(user->username[0] != '\n') {
      printf(BLUE"Username : %s\nAddress : %s"RESET"\n\n", user->username, user->address);

      //on avertis l'ui si elle est connectee
    	if (userInterface_fd > 0) {
    		char content[MSG_SIZE];
    		sprintf(content,"CONTACT %s\n",user->username);
    		sendUiMsg(content,readfds,fd_array,num_clients);
    	}

    }
  }

  free(user);
  close(contact_file);
  return 0;
}

int add_contact_online(client_data *fd_array, annuaireData * contact, int *num_clients, char * msg) {

  off_t offset;
  int contact_file;
  int new_file = -1;

  contact_file = open_directory();

  //On cherche si le contact existe, si ce n'est pas le cas on vérifie si le fichier est vide ou pas, puis on le créé, sinon on le modifie.
  if ((offset = search_contact(contact->username, contact_file)) == -1) {
    if(lseek(contact_file, 0, SEEK_END) < 0) {perror("[PROGRAM] Error while seeking end of contact file : file does not exist ?"); return -1; }
    write(contact_file, contact, sizeof(annuaireData));
    printf(BLUE"[PROGRAM] Connected contact added !"RESET"\n");
  }
  else {
    /* On supprime les anciennes données puis on réécrit les nouvelles */
    new_file = remove_contact_data(contact_file, offset);
    lseek(contact_file, 0, SEEK_END);
    if(write(new_file, contact, sizeof(annuaireData)) < 0) {perror("[PROGRAM] Error while writing contact update"); return -1;}
    printf(BLUE"[PROGRAM] Connected contact updated !"RESET"\n");
  }

  close(contact_file);
  close(new_file);
  return 0;
}

int print_connected_user(fd_set *readfds,client_data *fd_array, int *num_clients) {

	/* Fonction qui affiche l'ensemble des clients avec qui l'utilisateur est connecté */
	int i, j;
	printf("\n"BLUE"[PROGRAM] : You've connected yourself to the addresses \"0.0.0.0\""RESET"\n\n");
	printf(BLUE"Username\t\tAddress\t\t\tFD\n\n");

  //on avertis l'ui si elle est connectee
  if (userInterface_fd > 0) {
    char content[MSG_SIZE];
    sprintf(content,"WHOISCONNECTED %d\n",*num_clients-1);
    sendUiMsg(content,readfds,fd_array,num_clients);
  }

	for(i=0; i<*num_clients; i++) {
		printf(RED"%s\t\t\t%s \t\t%i\n", fd_array[i].name_client, fd_array[i].address_client, fd_array[i].fd_client);

    //on avertis l'ui si elle est connectee
  	if (userInterface_fd > 0 && fd_array[i].fd_client != userInterface_fd) {
  		char content[MSG_SIZE];
  		sprintf(content,"CONNECTED %s\n",fd_array[i].name_client);
  		sendUiMsg(content,readfds,fd_array,num_clients);
  	}

	}
	printf(RESET"\n");
	return 0;
}

off_t search_contact(char *name, int contact_list) {

  /*Cherche si le contact existe dans le fichier contact.txt, retourne l'offset du contact le cas échéant. */

  annuaireData *test = (annuaireData *) malloc(sizeof(annuaireData));
  int exist = 0;
  off_t offset = 0;

  lseek(contact_list, 0, SEEK_SET);

  while(exist == 0) { // sinon getline()
    if((read(contact_list, test, sizeof(annuaireData))) <= 0){ // voir si ça prend name ou name/195.65....
      exist = -1;
    } else if (strcmp(test->username, name) == 0){
      exist = 1;
    } else{
      offset += sizeof(annuaireData);
    }
  }
  free(test);

  if (exist == 1)
    return offset;
  else
    return -1;
}

int open_directory() {

  /* Fonction qui ouvre le fichier de contact */

  int fd;

  if (access(CONTACT, F_OK) == -1) {
    printf(BLUE"[PROGRAM] Contact file does not exist : creating file...\n");
    if ((fd = open(CONTACT, O_RDWR|O_CREAT, 0755)) < 0 ) {
      perror("[PROGRAM] Error while creating contact file\n");
    };
  }
  else {
    if ((fd = open(CONTACT, O_RDWR)) < 0) {
      perror("[PROGRAM] Error while opening contact file\n");
    }
  }

  return fd;
}

/* Fonction non utilisée */
void ask_contact_name(char *username){

  /*Fonction qui demande le nom de l'utilisateur, a compléter plus tard avec un système
  d'autentification plus complexe.*/

  char msg[100];
  int result;
  char buf[MAX_SIZE_USERNAME];

  printf(BLUE"[PROGRAM] Please enter contact name: "RESET"\n");
  do{
    memset (buf, '\0', sizeof(buf));//réinitialisation chaine
    if ((result = read(0, buf, WRITE_SIZE)) <= 0){
      perror("[PROGRAM] Name read error.");
      exit(EXIT_FAILURE);
      break;
    } else if(result > 16){ //on test result car sinon bug si l'utilisateur rentre + que 15, et > 16 car result compte le \n
      sprintf(msg, BLUE"[PROGRAM] Username too long, please enter another: "RESET"\n");
      write(0,msg, strlen(msg));
    } else if(result < 4){
      sprintf(msg, BLUE"[PROGRAM] Username too short, please enter another: "RESET"\n");
      write(0,msg, strlen(msg));
    }
  } while(result > 16 || result < 4);

  buf[strlen(buf)-1] = '\0';
  strcpy(username, buf);

}

int remove_contact_data(int original, off_t delete_line){

  /*Fonction qui permet de update les données d'un contact dans contact.txt.*/

  int new_file = -1;
  off_t offset = 0;
  int etat = 0;
  annuaireData *test = (annuaireData *) malloc(sizeof(annuaireData));

  //rewind
  if(lseek(original, 0, SEEK_SET) < 0) {
    free(test);
    return -1;
  }

  //open new file in write mode
  if ((new_file = open("temp.txt", O_WRONLY|O_CREAT, 0755)) < 0) {perror("Error while creating temporary file"); free(test); return -1; }
  if ((etat = read(original, test, sizeof(annuaireData))) < 0) {perror("Error while reading from original contact file"); free(test); return -1; }

  while (etat > 0)
  {
    if (offset != delete_line)
    {
      write(new_file, test, sizeof(annuaireData));
    }
    etat = read(original, test, sizeof(annuaireData));
    offset += sizeof(annuaireData);

    //except the line to be deleted
  }

  remove(CONTACT);
  rename("temp.txt", CONTACT);
  free(test);

  return new_file;
}

/* Fonction non utilisée */
void ask_contact_address(char * temp){

  char *posPort = NULL;
  char hostname[256]; //Changer 256, et surtout dans les fgets car cela peut poser des problèmes de sécurité
  int port = -1;

  while(strlen(hostname) == 0 || port == -1){
    fgets(hostname, 256, stdin);
    if((posPort = strchr(hostname, ':')) != NULL){
      port = (int) strtol(posPort+1, NULL, 10);
      if(port <= 0 || port >= 100000)
      port = -1;
    }else if(posPort == NULL || port == -1){
      printf(BLUE"-----Please enter correct address-----"RESET"\n");
      continue;
    }

    strncpy(temp, hostname, strlen(hostname));
    temp[strlen(temp)-1] = '\0';

  }
}

int connect_to_contact(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg, waitList *waitlist){

  int contact_file;
  int i;
  int bchar=0;
  char *posSpace = NULL;
  char name[MAX_SIZE_USERNAME];
  off_t offset = 0;
  annuaireData *test = (annuaireData *) malloc(sizeof(annuaireData));

  if(strlen(msg) < MIN_SIZE_USERNAME+9 || strlen(msg) > MAX_SIZE_USERNAME+9) {// "/connect " = 9
    printf(BLUE"[PROGRAM] Error command : if you want to use \"/connect username\" -> username too short or too long"RESET"\n");
    return -1;
  }
  if((posSpace = strchr(msg, ' ')) == NULL){
    printf(BLUE"[PROGRAM] Error command."RESET"\n");
    return -1;
  }

  strcpy(name, posSpace+1);
  name[strlen(name) - 1] = '\0';

  for(i=0 ; i < strlen(name) ; i++) {
    bchar = (isalnum((unsigned char) name[i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : if you want to use \"/connect username\" -> username must be composed of alphanumeric characters. "RESET"\n");
    return -1;
  }
  /* Fin de l'initialisation / gestion d'erreurs. */

  if ((contact_file = open_directory()) == -1 ) {
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("CONNECT Error opening contact file.\n",readfds,fd_array,num_clients);
    }
    free(test);
    return -1;
  }
  if((offset = search_contact(name, contact_file)) == -1){
    printf(BLUE"[PROGRAM] Contact doesn't exist. Please add it with \"/add username\"."RESET"\n");
    //on avertis l'ui si elle est connectee
    if (userInterface_fd > 0) {
      sendUiMsg("CONNECT Contact doesn't exist. Please add it with \"/add username\".\n",readfds,fd_array,num_clients);
    }
    free(test);
    return -1;
  }

  lseek(contact_file, offset, SEEK_SET);
  read(contact_file, test, sizeof(annuaireData));

  client(maxfds, readfds, num_clients, fd_array, test, waitlist);

  free(test);
  close(contact_file);

  return 0;
}
