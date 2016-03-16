#include "client_function.h"

struct hostent * ask_server_adress(int *port, annuaireData *user){

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
				printf("\n-----Please enter correct adress-----\n");
				continue;
			}

			strncpy(temp, hostname, ((int) strlen(hostname) - (int) strlen(posPort)));
			temp[((int) strlen(hostname) - (int) strlen(posPort))] = '\0';

			if((hostinfo = gethostbyname(temp)) == NULL)
			memset(hostname, 0, sizeof (hostname));
		}
	}else{
		posPort = strchr(user->adress, ':');
		*port = (int) strtol(posPort+1, NULL, 10);
		strncpy(temp, user->adress, ((int) strlen(user->adress) - (int) strlen(posPort)));
		temp[((int) strlen(user->adress) - (int) strlen(posPort))] = '\0';

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
	message *msg = (message *) malloc(sizeof(message));

	if(user != NULL){
		hostinfo = ask_server_adress(&port, user);
	}else{
		printf("\n*** Enter server's address : ***\n");
		hostinfo = ask_server_adress(&port, NULL);
	}

	printf("\n*** Client program starting (enter \"/quit\" to stop): ***\n");

	sock_host = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	/* Connection au serveur */
	if(connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0) { perror("connect"); exit(EXIT_FAILURE); } //gérer autrement car il ne faut pas quitter si on arrive pas a se co

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
        fd_array[*num_clients].fd_client=*client_sockfd;
        fd_array[*num_clients].id_client=*num_clients;
        fd_array[*num_clients].rdy = 0;
        strcpy(fd_array[*num_clients].name_client,user);
				(*num_clients)++;
        return 0;
    }else{
        printf("[PROGRAM] Session denied : %s already connected\n", user);
        session_denied(msg_send, 1);
				send_msg(msg_send, client_sockfd,readfds,fd_array,num_clients);
        close(*client_sockfd);
        FD_CLR(*client_sockfd, readfds);
        return -1;
    }
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
	for(i=0; i<*num_clients; i++){
		if(fd == fd_array[i].fd_client)
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
