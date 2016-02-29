#include "serv_function.h"

void ask_name(){
	char user[WRITE_SIZE]; //laisser a write size pour que ce soit cohérent avec le read
	char msg[100];
	int result;

	printf("Please enter your name: \n");
	do{
    memset (user, '\0', sizeof(user));//réinitialisation chaine
    if ((result = read(0, user, WRITE_SIZE)) <= 0){
			perror("Name read error.");
			exit(EXIT_FAILURE);
			break;
    }else if(result > 16){ //on test result car sinon bug si l'utilisateur rentre + que 15, et > 16 car result compte le \n
      sprintf(msg, "Username too long, please enter another: \n");
      write(0,msg, strlen(msg));
    }else if(result < 4){
      sprintf(msg, "Username too short, please enter another: \n");
      write(0,msg, strlen(msg));
    }
  }while(result > 16 || result < 4);
	user[strlen(user)-1] = '\0';
	strcpy(General_Name,user);

	printf("Your name is: %s\n",General_Name);

}

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	message msg_rcv;

	sscanf(msg, "%d/", &msg_rcv.code);

	if(protocol_parser(msg, &msg_rcv) != -1){
		printf("%s\n", msg_rcv.msg_content);

		switch(msg_rcv.code){
			case 200:
			login_client(msg_rcv.msg_content, client_sockfd, fd_array, num_clients, readfds);
			break;
			//case 201:

		}
	free(msg_rcv.msg_content);
	}
}

void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients){
	int i;
	close(fd);
	FD_CLR(fd, readfds); //on enlève le leaver du tableau de clients
	for (i = 0; i < (*num_clients) - 1; i++)
	 if (fd_array[i].fd_client == fd)
	   break;
	for (; i < (*num_clients) - 1; i++)
	 (fd_array[i].fd_client) = (fd_array[i + 1].fd_client);
	(*num_clients)--;
}

void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients){
	int i;
	char msg[WRITE_SIZE];
	printf("--- Server is shutting down\n");
	sprintf(msg, "X[SERVER] : Server is shutting down.\n");  // modifier le X pour dire au client de couper
	for (i = 0; i < *num_clients ; i++) {
		write(fd_array[i].fd_client, msg, strlen(msg));
		close(fd_array[i].fd_client);
	}
	close(*server_sockfd);
	exit(0);
}

void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char msg[WRITE_SIZE];
	int  result;

	int user_id = search_client_id(fd, fd_array, num_clients);

	if ((result = read(fd, msg, WRITE_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		msg[result] = '\0';

		printf("%s : %s",fd_array[user_id].name_client,msg);
	} else {
		printf("End of connection of client as %s\n", fd_array[user_id].name_client);  // a modifier avec le pseudo du mec
		exitClient(fd, readfds, fd_array, num_clients);
	} //if read

}

void handler_sigint(){

  /*Mise en plase du handler pour SIGINT*/
  /*On l'ignore car impossible de passer arguments a handler, sauf en passant par des variables globales*/
  /*On ne peut par contre pas intercepter SIGSTOP (ctrl-z) ou SIGKILL*/
  struct sigaction handler;
  memset(&handler, 0, sizeof(handler));
  handler.sa_handler = SIG_IGN;
  handler.sa_flags = 0;
  sigemptyset(&(handler.sa_mask));
  if(sigaction(SIGINT, &handler, NULL) != 0){
    perror("Error with sigaction");
    exit(EXIT_FAILURE);
  }

}

int * init_server(){

  struct sockaddr_in server_address;
	int addresslen = sizeof(struct sockaddr_in);
  int * server_sockfd = (int *) malloc(sizeof(int));;

  printf("\n*** Server program starting (enter \"quit\" to stop) ***\n");

  *server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = 0;
  if (bind(*server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("bind"); exit(EXIT_FAILURE); }
  if (getsockname(*server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("getsockname"); exit(EXIT_FAILURE); }

  printf("\n*** Connections available on port : %d ***\n", ntohs(server_address.sin_port));

  return server_sockfd;
}

void routine_server(int * server_sockfd){

  int num_clients = 0;
  int client_sockfd;
  struct sockaddr_in client_address;
  int addresslen = sizeof(struct sockaddr_in);
  int fd; //desc quand un client parle
  client_data fd_array[MAX_CLIENTS]; //tableau de data client
  fd_set readfds, testfds;
  int maxfds;
  char msg[WRITE_SIZE];

  maxfds = *server_sockfd;
  if(listen(*server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???
  FD_ZERO(&readfds);
  FD_SET(*server_sockfd, &readfds);
  FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

  /*  Attente de clients et de requêtes */
  while (1) {
    testfds = readfds;
    if(select(maxfds+1, &testfds, NULL, NULL, NULL) < 0) { perror("select"); exit(EXIT_FAILURE); }

    /* Il y a une activité, on cherche sur quel descripteur grâce à FD_ISSET */
    for (fd=0; fd<maxfds+1; fd++) {
      if (FD_ISSET(fd, &testfds)) {

        if (fd == *server_sockfd) { /* Accept des nouvelles connections */
          if((client_sockfd = accept(*server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&addresslen )) < 0 ) { perror("accept"); exit(EXIT_FAILURE); }

          if (num_clients < MAX_CLIENTS) {

            opt_desc(&client_sockfd, &maxfds, &readfds); //optimisation descripteurs

						sprintf(msg, "200/%s", General_Name); //1ere étape, on envois le nom du serveur
						write(client_sockfd, msg, sizeof(General_Name));

						memset (msg, '\0', sizeof (msg));//réinitialisation chaine

						read(client_sockfd, msg, sizeof(msg));
						rechercheProtocol(msg, &client_sockfd, fd_array, &num_clients, &readfds);

          } else {
            printf("--- Someone tried to connect, but too many clients online\n");
            sprintf(msg, "Sorry, too many clients online. Try again later.\n"); // modifier le X pour dire au client de couper
            write(client_sockfd, msg, strlen(msg));
            close(client_sockfd);
          } //if num_clients < MAX_CLIENTS

        } else if (fd == 0) {  /*activité sur le clavier*/
					cmde_host(&readfds, server_sockfd, &maxfds, fd_array, &num_clients);
        } else {  /*activité d'un client*/
          traiterRequete(fd, &readfds, fd_array, &num_clients);
        }//if fd ==
      }//if FD_ISSET
    }//for
  }//while
}

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

	char user[WRITE_SIZE];

	sscanf(msg, "%s", user);

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

void cmde_host(fd_set *readfds, int *server_sockfd, int *maxfds, client_data *fd_array, int *num_clients){

	char msg[WRITE_SIZE];
	char rep_msg[MSG_SIZE];
	int i;

	fgets(msg, WRITE_SIZE, stdin);
	if (strcmp(msg, "/quit\n")==0) {      // A arranger avec plus de tests : si longueur 4 et quit ou des trucs du genre
		quit_server(readfds, fd_array, server_sockfd, num_clients);
	} else if (strcmp(msg, "/connect\n")==0){
		client(maxfds, readfds, num_clients, fd_array);
	} else {
		sprintf(rep_msg, "%s", msg);
		for (i=0; i<*num_clients ; i++)
		write(fd_array[i].fd_client, rep_msg, strlen(rep_msg));
	}

}

int protocol_parser(char *msg, message *msg_rcv){
	char * sep = NULL;

	if((sep = strchr(msg, '/')) != NULL){
		(*msg_rcv).msg_content = malloc(sizeof(sep+1));
		strcpy((*msg_rcv).msg_content, sep+1);
		return 0;
	}

	return -1;
}
