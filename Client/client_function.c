#include "client_function.h"

struct hostent * ask_server_address(int *port, annuaireData *user){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[MAX_SIZE_ADDRESS]; //Changer 256, et surtout dans les fgets car cela peut poser des problèmes de sécurité
	char temp[MAX_SIZE_ADDRESS];
	int i, countpoint, countspace, countcolon;
	int bchar;

	if(user == NULL){
		while(strlen(hostname) == 0 || *port == -1 || countpoint != 3 || countspace > 1 || countcolon != 1) {
			fgets(hostname, WRITE_SIZE, stdin);

			if(strcmp(hostname, "/exit\n") == 0) {
				printf(BLUE"\n[PROGRAM] Getting out of connection routine."RESET"\n");
				*port = -1;
				break;
			}
			/* Vérification de la taille de l'adresse */
			if(strlen(hostname) < MIN_SIZE_ADDRESS || strlen(hostname) > MAX_SIZE_ADDRESS) {
				printf(BLUE"\n[PROGRAM] ----- Wrong length : please enter correct address -----"RESET"\n");
				continue;
			}
			/* Vérification de la syntaxe de l'adresse*/
			for (i=0; i < strlen(hostname); i++) {
				if(hostname[i] == '.') {countpoint++;}
				if(hostname[i] == ' ') {countspace++;}
				if(hostname[i] == ':') {countcolon++;}
				bchar = (isdigit((unsigned char) hostname[i]) || hostname[i] == '.' || hostname[i] == ':' || hostname[i] == ' ') ? 0 : 1;
				if(bchar) {break;}
			}
			if(countpoint != 3 || bchar || countspace > 1 || countcolon != 1) {
				printf(BLUE"\n[PROGRAM] ----- Wrong syntax : please enter correct address -----"RESET"\n");
				continue;
			}
			/* Vérification du port */
			if((posPort = strchr(hostname, ' ')) != NULL){
				*port = (int) strtol(posPort, NULL, 10);
				if(*port <= 0 || *port >= 100000 || ((strncmp(hostname, "0.0.0.0", 7) == 0) && *port == General_Port))
				*port = -1;
			} else if ((posPort = strchr(hostname, ':')) != NULL){
				*port = (int) strtol(posPort+1, NULL, 10);
				if(*port <= 0 || *port >= 100000 || ((strncmp(hostname, "0.0.0.0", 7) == 0) && *port == General_Port))
				*port = -1;
			}
			if(posPort == NULL || *port == -1){
				printf(BLUE"\n[PROGRAM] -----Please enter correct address-----"RESET"\n");
				continue;
			}

			strncpy(temp, hostname, ((int) strlen(hostname) - (int) strlen(posPort)));
			temp[((int) strlen(hostname) - (int) strlen(posPort))] = '\0';

			if((hostinfo = gethostbyname(temp)) == NULL)
			memset(hostname, 0, sizeof (hostname));
		}
	} else {

		posPort = strchr(user->address, ':');
		*port = (int) strtol(posPort+1, NULL, 10);
		strncpy(temp, user->address, ((int) strlen(user->address) - (int) strlen(posPort)));
		temp[((int) strlen(user->address) - (int) strlen(posPort))] = '\0';

		if((hostinfo = gethostbyname(temp)) == NULL)
		memset(hostname, 0, sizeof (hostname));
	}

	return hostinfo;
}

int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, annuaireData *user, waitList *waitlist){
	int sock_host;
	struct hostent *hostinfo;
	struct sockaddr_in address;
	int port = -1;
	char client_inaddr[INET_ADDRSTRLEN];
	message *msg = (message *) malloc(sizeof(message));

	//Si on ne peut pas recevoir le client
	if (*num_clients >= MAX_CLIENTS) {
		printf(BLUE"[Program] You tried to establish a connection with someone, but you are already connected to too many clients."RESET"\n\n");
		return -1;
	}

	if(user != NULL){
		hostinfo = ask_server_address(&port, user);
	}else{
		printf(BLUE"\n[PROGRAM] *** Enter server's address : \"address:port\" ***\n[PROGRAM] Type \"/exit\" to interrupt."RESET"\n\n");
		hostinfo = ask_server_address(&port, NULL);
	}
	/* Cas où l'utilisateur veut quitter la routine de connexion */
	if(port == -1) {
		return -1;
	}

	sock_host = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_ntop(AF_INET, &(address.sin_addr), client_inaddr, INET_ADDRSTRLEN);
	printf(BLUE"\n*** Attempt to establish a connection with "RED"%s:%i"BLUE" ***"RESET"\n\n", client_inaddr, port);

	/* Connection au serveur */
	if(connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Connect");
		return -1;
	} //gérer autrement car il ne faut pas quitter si on arrive pas a se co

	opt_desc(&sock_host, maxfds, readfds);
	session_initiate(msg); //génération du message de session-initiate
	send_msg(msg,&sock_host,readfds,fd_array,num_clients);

	((*waitlist).nb_connect)++;
	/* Ajout de l'adresse socket du client auquel on se connecte à ses données */
	memset(fd_array[*num_clients+(*waitlist).nb_connect].address_client, '\0', sizeof(fd_array->address_client));
	strcpy(fd_array[*num_clients+(*waitlist).nb_connect].address_client, client_inaddr);
	fd_array[*num_clients+(*waitlist).nb_connect].fd_client = sock_host;
	(*waitlist).waiting[(*waitlist).nb_connect]=sock_host;

	free((*msg).msg_content);
	free(msg);

	return 0;
}//main

void opt_desc(int *client_sockfd, int *maxfds, fd_set *readfds){

  /* Ajouter client_sockfd dans la liste des descripteurs en attente */
  /* Pour cela, on cherche si un descripteur plus petit est libre */
  /* Optimisation de descripteurs*/

  int descDup;
  descDup = dup(*client_sockfd);
  if (*client_sockfd < descDup) {
    close(descDup);
    *maxfds = Max(*client_sockfd, *maxfds);
    FD_SET(*client_sockfd, readfds);
  } else {
    close(*client_sockfd);
    *maxfds = Max(descDup, *maxfds);
    FD_SET(descDup, readfds);
  }

}

int login_client(message *msg_send, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist) {

	/*Fonction qui permet a un client de se connecter, ses paramètres sont enregistré dans
	une structure client_data puis ajouté a fd_array, qui contient les infos sur tous les
	autres clients.*/

	int i;
	int client_id;
	int waiting_ind, waiting_arr;

	waiting_ind = search_client_waiting_array_by_fd(*client_sockfd, fd_array, num_clients, *waitlist);
	waiting_arr = search_waiting_array_by_fd(*client_sockfd, *waitlist);

  if (search_client_id_by_name(fd_array[*num_clients+(*waitlist).nb_connect].name_client, fd_array, num_clients) == -1) { //si on a pas de conversation déjà commencée avec le client
		/* Enregistrement des paramètres */
		fd_array[*num_clients].fd_client = *client_sockfd;
  	fd_array[*num_clients].id_client = *num_clients;
  	fd_array[*num_clients].rdy = 0;
    strcpy(fd_array[*num_clients].name_client, fd_array[waiting_ind].name_client);
		strcpy(fd_array[*num_clients].address_client, fd_array[waiting_ind].address_client);
		/* Suppression des paramètres pré-enregistrés si les index sont différents */
		if(waiting_ind == *num_clients+(*waitlist).nb_connect) {
			memset(fd_array[waiting_ind].address_client, '\0', sizeof(fd_array->address_client));
			memset(fd_array[waiting_ind].name_client, '\0', sizeof(fd_array->name_client));
			fd_array[waiting_ind].fd_client = 0;
		}
		/* Modification des compteurs */
		(*num_clients)++;
		((*waitlist).nb_connect)--;
    return 0;

  } else {
    printf(BLUE"[PROGRAM] Session denied : "RED"%s"BLUE" already connected"RESET"\n\n", fd_array[waiting_ind].name_client);
    session_denied(msg_send, 1);
		send_msg(msg_send, client_sockfd,readfds,fd_array,num_clients);
		/* Suppression des paramètres pré-enregistrés si les index sont différents */
		memset(fd_array[waiting_ind].address_client, '\0', sizeof(fd_array->address_client));
		memset(fd_array[waiting_ind].name_client, '\0', sizeof(fd_array->name_client));
		fd_array[waiting_ind].fd_client = 0;
    close(*client_sockfd);
    FD_CLR(*client_sockfd, readfds);
		((*waitlist).nb_connect)--;
		(*waitlist).waiting[waiting_arr]=0;
		free((*msg_send).msg_content);
		free(msg_send);
    return -1;

  }
}

int connect_refuse(client_data *fd_array, int *num_clients, fd_set *readfds, char *msg, waitList *waitlist) {

	/* Fonction qui permet à un client de refuser une connextion d'un utilisateur distant. */

	char client_name[MAX_SIZE_USERNAME];
	int client_sockfd;
	int waiting_ind, waiting_arr;
	int i, bchar;
	char *posSpace = NULL;

	if(strlen(msg) < MIN_SIZE_USERNAME+8 || strlen(msg) > MAX_SIZE_USERNAME+8) { // "/refuse " = 8
		printf(BLUE"[PROGRAM] Error command. Please use \"/refuse username\" as described previously."RESET"\n\n");
		return -1;
	}
	if((posSpace = strchr(msg, ' ')) == NULL) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/refuse username\" as described previously."RESET"\n\n");
    return -1;
  }
	if(strcpy(client_name, posSpace+1) == NULL) {
		printf(BLUE"[PROGRAM] Error command. Please use \"/refuse username\" as described previously."RESET"\n\n");
		return -1;
	}
	for(i=0 ; i < strlen(client_name) ; i++) {
    bchar = (isalnum((unsigned char) client_name[i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : Username must only be composed of alphanumeric characters.\n[PROGRAM] Please use \"/refuse username\" as described previously.\n"RESET"\n\n");
    return -1;
  }
	client_name[strlen(client_name)-1] = '\0';

	if((client_sockfd = search_client_waiting_fd_by_name(client_name, fd_array, num_clients, *waitlist)) == -1) {
		printf(BLUE"[PROGRAM] This user is not in your waiting list."RESET"\n\n");
    return -1;
	}
	/* Fin de l'initialisation / gestion d'erreurs : on alloue de la mémoire */
	message *msg_send = (message *) malloc(sizeof(message));
	waiting_ind = search_client_waiting_array_by_fd(client_sockfd, fd_array, num_clients, *waitlist);
	waiting_arr = search_waiting_array_by_fd(client_sockfd, *waitlist);

	printf(BLUE"[PROGRAM] Session not established : you refused the connection with "RED"%s."RESET"\n", client_name);
  session_denied(msg_send, 2);
  send_msg(msg_send, &client_sockfd, readfds, fd_array, num_clients);
	/* Suppression des paramètres pré-enregistrés si les index sont différents */
	memset(fd_array[waiting_ind].address_client, '\0', sizeof(fd_array->address_client));
	memset(fd_array[waiting_ind].name_client, '\0', sizeof(fd_array->name_client));
	fd_array[waiting_ind].fd_client = 0;
	close(client_sockfd);
  FD_CLR(client_sockfd, readfds);
	((*waitlist).nb_connect)--;
	(*waitlist).waiting[waiting_arr]=0;
	free((*msg_send).msg_content);
	free(msg_send);
	return 0;
}

int connect_accept(client_data *fd_array, int *num_clients, fd_set *readfds, char *msg, waitList *waitlist) {

	/* Fonction qui permet à un client d'accepter une connextion d'un utilisateur distant. */

	char client_name[MAX_SIZE_USERNAME];
	int client_sockfd;
	int i, bchar;
	char *posSpace = NULL;

	if(strlen(msg) < MIN_SIZE_USERNAME+8 || strlen(msg) > MAX_SIZE_USERNAME+8) { // "/accept " = 8
		printf(BLUE"[PROGRAM] Error command : Username too short or too long.\n[PROGRAM] Please use \"/accept username\" as described previously."RESET"\n\n");
		return -1;
	}
	if((posSpace = strchr(msg, ' ')) == NULL) {
    printf(BLUE"[PROGRAM] Error command. Please use \"/accept username\" as described previously."RESET"\n\n");
    return -1;
  }
	if(strcpy(client_name, posSpace+1) == NULL) {
		printf(BLUE"[PROGRAM] Error command. Please use \"/accept username\" as described previously."RESET"\n\n");
		return -1;
	}
	for(i=0 ; i < strlen(client_name) ; i++) {
    bchar = (isalnum((unsigned char) client_name[i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : Username must only be composed of alphanumeric characters.\n[PROGRAM] Please use \"/accept username\" as described previously.\n"RESET"\n\n");
    return -1;
  }
	client_name[strlen(client_name)-1] = '\0';

	if((client_sockfd = search_client_waiting_fd_by_name(client_name, fd_array, num_clients, *waitlist)) == -1) {
		printf(BLUE"[PROGRAM] This user is not in your waiting list."RESET"\n\n");
    return -1;
	}
	/* Fin de l'initialisation / gestion d'erreurs : on alloue de la mémoire */

	message *msg_send = (message *) malloc(sizeof(message));
	//On lui demande de se logger
  if (login_client(msg_send, &client_sockfd, fd_array, num_clients, readfds, waitlist) != -1) {
    //On confirme la connection du client
    session_accept(msg_send); //on crée le message de session-accept-1
    send_msg(msg_send, &client_sockfd, readfds, fd_array, num_clients);
    free((*msg_send).msg_content);
		free(msg_send);
  }
	return 0;
}

int disconnect (int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg) {

	char *posSpace = NULL;
	char name[MAX_SIZE_USERNAME];
	int i, bchar;
	int client_sockfd;

	if(strlen(msg) < MIN_SIZE_USERNAME+12 || strlen(msg) > MAX_SIZE_USERNAME+12) { // "/disconnect " = 12
		printf(BLUE"[PROGRAM] Error command : Username too short or too long.\n[PROGRAM] Please use \"/disconnect username\"."RESET"\n\n");
		return -1;
	}

	if((posSpace = strchr(msg, ' ')) == NULL){
    printf(BLUE"[PROGRAM] Error command. Please use \"/disconnect username\"."RESET"\n\n");
    return -1;
  }
	if(strcpy(name, posSpace+1) == NULL) {
		printf(BLUE"[PROGRAM] Error command. Please use \"/disconnect username\"."RESET"\n\n");
		return -1;
	}
	for(i=0 ; i < strlen(name) ; i++) {
    bchar = (isalnum((unsigned char) name[i])) ? 0 : 1;
    if(bchar) {break;}
  }
  if (bchar) {
    printf(BLUE"[PROGRAM] Error command : Username must only be composed of alphanumeric characters.\n[PROGRAM] Please use \"/disconnect username\".\n"RESET"\n\n");
    return -1;
  }
	name[strlen(name) - 1] = '\0';

	if((client_sockfd = search_client_fd_by_name(name, fd_array, num_clients)) < 0) {
		printf(BLUE"Client is not connected"RESET"\n\n");
		return -1;
	}
	/* Fin de l'initialisation / gestion d'erreurs : on alloue de la mémoire */
	message *discomsg = (message *) malloc(sizeof(message));

	session_end(discomsg);
	send_msg(discomsg, &client_sockfd, readfds, fd_array, num_clients);
	exitClient(client_sockfd, readfds, fd_array, num_clients);
	printf(BLUE"["RED"%s"BLUE"] End of connection."RESET"\n", name);
	free((*discomsg).msg_content);
	free(discomsg);
	return 0;

}

void client_ready(int fd, client_data *fd_array, int *num_clients) {

/* Met le client en ready, maintenant on peut lui parler */
    int i = search_client_array_by_fd(fd, fd_array, num_clients);
    fd_array[i].rdy = 1;
}

int search_client_ready_by_fd(int fd, client_data *fd_array, int *num_clients) {

/*Fonction qui prend un file descriptor et renvoie si le client est ready*/

    int i = 0;
    if ((i = search_client_array_by_fd(fd, fd_array, num_clients)) != -1) {
        if (fd_array[i].rdy == 1)
            return 0;
    }
    return -1;
}

int search_client_array_by_fd(int fd, client_data *fd_array, int *num_clients) {

/*Fonction qui prend un file descriptor et renvoie l'indice dans fd_array correspondant au client s'il existe*/

	int i;
	for(i=0; i < *num_clients; i++){
		if(fd == fd_array[i].fd_client)
			return i;
	}
	return -1;
}

int search_client_waiting_array_by_fd(int fd, client_data *fd_array, int *num_clients, waitList waitlist) {

/*Fonction qui prend un file descriptor et renvoie l'indice dans fd_array correspondant au client en attente s'il existe*/

	int i;
	for(i=0; i <= *num_clients+waitlist.nb_connect; i++){
		if(fd == fd_array[i].fd_client)
			return i;
	}
	return -1;
}

int search_client_waiting_fd_by_name(char *user, client_data *fd_array, int *num_clients, waitList waitlist) {

/*Fonction qui prend un file descriptor et renvoie l'indice dans fd_array correspondant au client en attente s'il existe*/

	int i;
	for(i=0; i <= *num_clients+waitlist.nb_connect; i++){
		if(strcmp(user, fd_array[i].name_client) == 0) {
			return fd_array[i].fd_client;
		}
	}
	return -1;
}

int search_waiting_array_by_fd(int fd, waitList waitlist) {

/*Fonction qui prend un file descriptor et renvoie l'indice dans waitlist correspondant au client en attente s'il existe*/

	int i;
	for(i=0; i < waitlist.nb_connect; i++){
		if(fd == waitlist.waiting[i])
			return i;
	}
	return -1;
}

int search_client_array_by_name(char *user, client_data *fd_array, int *num_clients) {

/*Fonction qui prend un username de client et renvoie l'indice dans fd_array correspondant au client s'il existe*/

	int i;
	for(i=0; i<*(num_clients); i++){
		if(user == fd_array[i].name_client)
			return i;
	}
	return -1;
}

int search_client_id_by_fd(int fd, client_data *fd_array, int *num_clients) {

/*Fonction qui prend un file descriptor et renvoie l'ID du client s'il existe*/

	int i;
	for(i=0; i<*num_clients; i++){
		if(fd == fd_array[i].fd_client)
			return fd_array[i].id_client;
	}
	return -1;
}

int search_client_id_by_name(char *user, client_data *fd_array, int *num_clients) {

	/*Fonction qui prend un username de client et renvoie l'ID du client s'il existe*/

	int i;
	for(i=0; i<*num_clients; i++){
		if(strcmp(user, fd_array[i].name_client) == 0)
			return fd_array[i].id_client;
	}
	return -1;
}

int search_client_fd_by_name(char *user, client_data *fd_array, int *num_clients) {

	/*Fonction qui prend un username de client et renvoie le fd du client s'il existe*/

	int i;
	for(i=0; i<*num_clients; i++){
		if(strcmp(user, fd_array[i].name_client) == 0)
			return fd_array[i].fd_client;
	}
	return -1;
}

char * search_client_address_by_name(char *user, client_data *fd_array, int *num_clients) {

	/*Fonction qui prend un username de client et renvoie l'adresse du client s'il existe*/

	int i;
	for(i=0; i<*num_clients; i++){
		if(strcmp(user, fd_array[i].name_client) == 0) {
			return fd_array[i].address_client;
		}
	}
	return NULL;
}
