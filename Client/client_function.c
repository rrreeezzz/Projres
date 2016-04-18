#include "client_function.h"

struct hostent * ask_server_address(int *port, annuaireData *user){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[256]; //Changer 256, et surtout dans les fgets car cela peut poser des problèmes de sécurité
	char temp[256];

	if(user == NULL){
		while(strlen(hostname) == 0 || *port == -1){
			fgets(hostname, 256, stdin);
			if((posPort = strchr(hostname, ' ')) != NULL){
				*port = (int) strtol(posPort, NULL, 10);
				if(*port <= 0 || *port >= 100000)
				*port = -1;
			}else if((posPort = strchr(hostname, ':')) != NULL){
				*port = (int) strtol(posPort+1, NULL, 10);
				if(*port <= 0 || *port >= 100000)
				*port = -1;
			}else if(posPort == NULL || *port == -1){
				printf("\n-----Please enter correct address-----\n");
				continue;
			}

			strncpy(temp, hostname, ((int) strlen(hostname) - (int) strlen(posPort)));
			temp[((int) strlen(hostname) - (int) strlen(posPort))] = '\0';

			if((hostinfo = gethostbyname(temp)) == NULL)
			memset(hostname, 0, sizeof (hostname));
		}
	}else{
		posPort = strchr(user->address, ':');
		*port = (int) strtol(posPort+1, NULL, 10);
		strncpy(temp, user->address, ((int) strlen(user->address) - (int) strlen(posPort)));
		temp[((int) strlen(user->address) - (int) strlen(posPort))] = '\0';

		if((hostinfo = gethostbyname(temp)) == NULL)
		memset(hostname, 0, sizeof (hostname));
	}

	return hostinfo;
}

int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, annuaireData *user){
	int sock_host;
	struct hostent *hostinfo;
	struct sockaddr_in address;
	int port = -1;
	char client_inaddr[INET_ADDRSTRLEN];
	message *msg = (message *) malloc(sizeof(message));

	if(user != NULL){
		hostinfo = ask_server_address(&port, user);
	}else{
		printf("\n*** Enter server's address : ***\n");
		hostinfo = ask_server_address(&port, NULL);
	}

	printf("\n*** Client program starting (enter \"/quit\" to stop): ***\n");

	sock_host = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	/* Connection au serveur */
	if(connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("Connect");
		return -1;
	} //gérer autrement car il ne faut pas quitter si on arrive pas a se co

	/* Ajout de l'adresse socket du client auquel on se connecte à ses données */
	inet_ntop(AF_INET, &(address.sin_addr), client_inaddr, INET_ADDRSTRLEN);
	memset(fd_array[*num_clients].address_client, '\0', sizeof(fd_array->address_client));
	strcpy(fd_array[*num_clients].address_client, client_inaddr);

	opt_desc(&sock_host, maxfds, readfds);
	session_initiate(msg); //génération du message de session-initiate
	send_msg(msg,&sock_host,readfds,fd_array,num_clients);

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

int login_client(message *msg_rcv, message *msg_send, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) {

	/*Fonction qui permet a un client de se connecter, ses paramètres sont enregistré dans
	une structure client_data puis ajouté a fd_array, qui contient les infos sur tous les
	autres clients.*/

	char user[MAX_SIZE_USERNAME];
  sscanf(msg_rcv->msg_content, "%s", user);

  if (search_client_id_by_name(user, fd_array, num_clients) == -1) { //si on a pas de conversation déjà commencée avec le client
		fd_array[*num_clients].fd_client = *client_sockfd;
  	fd_array[*num_clients].id_client = *num_clients;
  	fd_array[*num_clients].rdy = 0;
    strcpy(fd_array[*num_clients].name_client, user);
		//strcpy(fd_array[*num_clients].address_client, fd_array->address_client);
		(*num_clients)++;
    return 0;
  } else {
    printf("[PROGRAM] Session denied : %s already connected\n", user);
    session_denied(msg_send, 1);
		send_msg(msg_send, client_sockfd,readfds,fd_array,num_clients);
    close(*client_sockfd);
    FD_CLR(*client_sockfd, readfds);
    return -1;
  }
}

int control_accept(message *msg_rcv, client_data *fd_array, int *num_clients) {

	/* Fonction qui permet à un client d'accepter ou pas une connextion d'un utilisateur
	distant. */
	char tmpAccept[WRITE_SIZE];
	char acceptConnection[WRITE_SIZE];

	printf("\n[PROGRAM] %s : %s is trying to establish a connection with you. Do you accept ? Type without caps \"yes\" to accept or \"no\" to refuse.\n", (*msg_rcv).msg_content, fd_array[*num_clients].address_client);
	fgets(tmpAccept, WRITE_SIZE, stdin);
	if(strcmp(tmpAccept, "yes\n") == 0) return 0;
	else return -1;

}

int disconnect (int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array, char *msg) {

	char *posSpace = NULL;
	char name[MAX_SIZE_USERNAME];
	int i;
	int client_sockfd;
	message *discomsg = (message *) malloc(sizeof(message));

	if((posSpace = strchr(msg, ' ')) == NULL){
    printf("[PROGRAM] Error command.");
    return -1;
  }
	strcpy(name, posSpace+1);
	name[strlen(name) - 1] = '\0';

	if((client_sockfd = search_client_fd_by_name(name, fd_array, num_clients)) < 0) {
		printf("Client is not connected\n");
		return -1;
	}

	session_end(discomsg);
	send_msg(discomsg, &client_sockfd, readfds, fd_array, num_clients);
	exitClient(client_sockfd, readfds, fd_array, num_clients);
	printf("[%s] End of connection.\n", name);
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
	for(i=0; i<*(num_clients); i++){
		if(fd == fd_array[i].fd_client)
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
