#include "client_function.h"

struct hostent * ask_server_adress(int *port){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[256]; //Changer 256, et surtout dans les fgets car cela peut poser des problèmes de sécurité
	char temp[256];

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

	return hostinfo;
}

int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array){
	int sock_host;
	struct hostent *hostinfo;
	struct sockaddr_in address;
	char msg[MSG_SIZE];
	int port = -1;
	message segment;

	printf("\n*** Enter server's address : ***\n");

	hostinfo = ask_server_adress(&port);

	printf("\n*** Client program starting (enter \"/quit\" to stop): ***\n");

	sock_host = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	/* Connection au serveur */
	if(connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0) { perror("connect"); exit(EXIT_FAILURE); } //gérer autrement car il ne faut pas quitter si on arrive pas a se co

	opt_desc(&sock_host, maxfds, readfds);

	login_msg(&segment); //génération du message de login
	send_msg(&segment, &sock_host); //on envois le message
	free(segment.msg_content);

	//read(sock_host, msg, sizeof(msg));
	//rechercheProtocol(msg, &sock_host, fd_array, num_clients, readfds);

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

void login_client(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds){

	char user[MAX_SIZE_USERNAME];
	sscanf(msg, "FROM:%s", user);

  if(search_client_name(user, fd_array, num_clients) == -1){ //si on a pas de conversation déjà commencé avec le client
    fd_array[*num_clients].fd_client=*client_sockfd;
		fd_array[*num_clients].id_client=*num_clients;
		strcpy(fd_array[*num_clients].name_client,user);
		(*num_clients)++;
    printf("You are now in communication with : %s\n", user);
  }else{
    close(*client_sockfd);
		FD_CLR(*client_sockfd, readfds);
  }
}

int search_client_id(int fd, client_data *fd_array, int *num_clients){
	int i;
	for(i=0; i<*num_clients; i++){
		if(fd == fd_array[i].fd_client)
			return fd_array[i].id_client;
	}
	return -1; //pas exit failure car fct qui peut surement resservir
}

int search_client_name(char *user, client_data *fd_array, int *num_clients){
	int i;
	for(i=0; i<*num_clients; i++){
		if(strcmp(user, fd_array[i].name_client) == 0)
			return fd_array[i].id_client;
	}
	return -1; //pas exit failure car fct qui peut surement resservir
}
