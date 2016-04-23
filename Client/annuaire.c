#include "annuaire.h"

int update_contact () {

  /*Permet d'ajouter un contact ou de mettre la base de données a jour.*/

  off_t offset;
  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
  int contact_file;
  int new_file = -1;

  contact_file = open_directory();

  ask_contact_name(user->username);

  printf(BLUE"[PROGRAM] Please enter contact address ("IBLUE"0.0.0.0:12345"BLUE"): "RESET"\n");

  ask_contact_address(user->address);

  //On cherche si le contact existe, si ce n'est pas le cas on vérifie si le fichier est vide ou pas, puis on le créé, sinon on le modifie.
  if ((offset = search_contact(user->username, contact_file)) == -1) {
    if(lseek(contact_file, 0, SEEK_END) < 0) {perror("[PROGRAM] Error while seeking end of contact file : file does not exist ?"); exit(EXIT_FAILURE); }
    write(contact_file, user, sizeof(annuaireData));
    printf(BLUE"[PROGRAM] Contact added !"RESET"\n");
  }
  else {
    /* On supprime les anciennes données puis on réécrit les nouvelles */
    new_file = remove_contact_data(contact_file, offset);
    lseek(contact_file, 0, SEEK_END);
    if(write(new_file, user, sizeof(annuaireData)) < 0) {perror("[PROGRAM] Error while writing contact update"); exit(EXIT_FAILURE);}
    printf(BLUE"[PROGRAM] Contact updated !"RESET"\n");
  }

  free(user);
  close(contact_file);
  close(new_file);
  return 0;
}

int remove_contact () {

  off_t offset;
  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
  int contact_file;

  contact_file = open_directory();

  ask_contact_name(user->username);

  if ((offset = search_contact(user->username, contact_file)) == -1) { perror("Error : contact was not found in contact list"); exit(EXIT_FAILURE); }
  if (remove_contact_data(contact_file, offset) == -1) {perror("Error while removing contact's data"); return -1; }
  printf(BLUE"[PROGRAM] Contact removed !"RESET"\n");

  free(user);
  close(contact_file);
  return 0;
}

int print_contact_list () {

  annuaireData * user = (annuaireData *) malloc(sizeof(annuaireData));
  int contact_file;
  char buf[sizeof(user)];

  contact_file = open_directory();

  while (read(contact_file, user, sizeof(annuaireData)) > 0) {
    if(user->username[0] != '\n') {
      printf(BLUE"Username : %s\nAddress : %s"RESET"\n\n", user->username, user->address);
    }
  }

  free(user);
  close(contact_file);
  return 0;
}

int add_contact_online(client_data *fd_array, int *num_clients, char * msg) {

  off_t offset;
  annuaireData * contact = (annuaireData *) malloc(sizeof(annuaireData));
  int contact_file;
  int new_file = -1;
  char name[MAX_SIZE_USERNAME];
  char * posSpace = NULL;
  char * contact_addr;

  if((posSpace = strchr(msg, ' ')) == NULL){
    printf(BLUE"[PROGRAM] Error command."RESET"\n");
    return -1;
  }
  if ((strlen(posSpace) < 4) && (strlen(posSpace) > 16)) {perror("[PROGRAM] : Username is wrong"); return -1;}
  strcpy(name, posSpace+1);
  name[strlen(name) - 1] = '\0';

  /* Fonction qui ajoute un contact avec qui l'on est déjà connecté */
  contact_addr = search_client_address_by_name(name, fd_array, num_clients);
  if (contact_addr == NULL) {
    printf(BLUE"[PROGRAM] This user is not connected, to add him, use /add then enter his name and address"RESET"\n");
    return -1;
  }
  else {

    contact_file = open_directory();

    strcpy(contact->username, name);
    strcpy(contact->address, contact_addr);

    //On cherche si le contact existe, si ce n'est pas le cas on vérifie si le fichier est vide ou pas, puis on le créé, sinon on le modifie.
    if ((offset = search_contact(contact->username, contact_file)) == -1) {
      if(lseek(contact_file, 0, SEEK_END) < 0) {perror("[PROGRAM] Error while seeking end of contact file : file does not exist ?"); exit(EXIT_FAILURE); }
      write(contact_file, contact, sizeof(annuaireData));
      printf(BLUE"[PROGRAM] Connected contact added !"RESET"\n");
    }
    else {
      /* On supprime les anciennes données puis on réécrit les nouvelles */
      new_file = remove_contact_data(contact_file, offset);
      lseek(contact_file, 0, SEEK_END);
      if(write(new_file, contact, sizeof(annuaireData)) < 0) {perror("[PROGRAM] Error while writing contact update"); exit(EXIT_FAILURE);}
      printf(BLUE"[PROGRAM] Connected contact updated !"RESET"\n");
    }
  }
  close(contact_file);
  close(new_file);
  return 0;
}

int print_connected_user(client_data *fd_array, int *num_clients) {

	/* Fonction qui affiche l'ensemble des clients avec qui l'utilisateur est connecté */
	int i, j;
	printf("\n"BLUE"[PROGRAM] : You've connected yourself to the addresses \"0.0.0.0\""RESET"\n\n");
	printf(BLUE"Username\t\tAddress\t\t\tFD\n\n");
	for(i=0; i<*num_clients; i++) {
		printf(RED"%s\t\t\t%s \t\t%i\n", fd_array[i].name_client, fd_array[i].address_client, fd_array[i].fd_client);
	}
	printf(RESET"\n");
	return 0;
}

off_t search_contact(char *name, int contact_list) {

  /*Cherche si le contact existe dans le fichier contact.txt.*/

  annuaireData *test = (annuaireData *) malloc(sizeof(annuaireData));
  int exist = 0;
  off_t offset = 0;

  lseek(contact_list, 0, SEEK_SET);

  while(exist == 0) { // sinon getline()
    if((read(contact_list, test, sizeof(annuaireData))) <= 0){ // voir si ça prend name ou name/195.65....
      exist = -1;
    }else if (strcmp(test->username, name) == 0){
      exist = 1;
    }else{
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
    }else if(result > 16){ //on test result car sinon bug si l'utilisateur rentre + que 15, et > 16 car result compte le \n
    sprintf(msg, BLUE"[PROGRAM] Username too long, please enter another: "RESET"\n");
    write(0,msg, strlen(msg));
  }else if(result < 4){
    sprintf(msg, BLUE"[PROGRAM] Username too short, please enter another: "RESET"\n");
    write(0,msg, strlen(msg));
  }
}while(result > 16 || result < 4);

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
  lseek(original, 0, SEEK_SET);

  //open new file in write mode
  if ((new_file = open("temp.txt", O_WRONLY|O_CREAT, 0755)) < 0) {perror("Error while creating temporary file"); return -1; }
  if ((etat = read(original, test, sizeof(annuaireData))) < 0) {perror("Error while reading from original contact file"); return -1; }

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
  char *posSpace = NULL;
  char name[MAX_SIZE_USERNAME];
  off_t offset = 0;
  annuaireData *test = (annuaireData *) malloc(sizeof(annuaireData));

  if((posSpace = strchr(msg, ' ')) == NULL){
    printf(BLUE"[PROGRAM] Error command."RESET"\n");
    return -1;
  }

  strcpy(name, posSpace+1);
  name[strlen(name) - 1] = '\0';

  contact_file = open_directory();

  if((offset = search_contact(name, contact_file)) == -1){
    printf(BLUE"[PROGRAM] Contact doesn't exist. Please add it with /add command."RESET"\n");
    return -1;
  }

  lseek(contact_file, offset, SEEK_SET);
  read(contact_file, test, sizeof(annuaireData));

  client(maxfds, readfds, num_clients, fd_array, test, waitlist);

  free(test);
  close(contact_file);

  return 0;
}
