#include "server_function.h"
#include "db.h"
#include "utilities.h"

int rechercheCmd(const char *msg) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!
	if (*msg == '/') {
		if (!strncmp(msg+1, "quit", 4)) { return QUIT;
		} else if (!strncmp(msg+1, "msg", 3)) { return MSG;
		} else if (!strncmp(msg+1, "grp", 3)) { return GRP;
		} else { return ALL1; }
	} else { return ALL2; }
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
	exit_mysql();
	exit(0);
}

void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char msg[WRITE_SIZE];
	char rep_msg[MSG_SIZE];
	int i, result;
	char *user_name;
	int user_id;

	user_id = search_client(fd, fd_array, num_clients);
	user_name = name_user_mysql(user_id);

	if ((result = read(fd, msg, WRITE_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		msg[result] = '\0';
		printf("Client %d as %s: %s", user_id, user_name, msg); // a modifier avec le pseudo du mec, on rajoute le '\n' du coup

		switch(rechercheCmd(msg)) {
			case QUIT:
			printf("--- Client %d as %s left\n", user_id, user_name);  // a modifier avec le pseudo du mec
			exitClient(fd, readfds, fd_array, num_clients);
			break;
			case MSG:
			//on envoie que au client concerné   rajouter notre structure en param de la fonction
			break;
			case GRP:
			//on envoie au groupe
			break;
			case ALL1: // comande /all
			sprintf(rep_msg, "%s : %s", user_name, msg+5); //msg+5 pour enlever le /all
			for(i=0; i<(*num_clients); i++){
				if (fd_array[i].fd_client != fd)  /* on ne renvoie pas à l'emetteur */
				if ((result = write(fd_array[i].fd_client, rep_msg, strlen(rep_msg))) < 0) { perror("write ALL1"); exit(EXIT_FAILURE); }
			}
			break;
			case ALL2: //ici pas de /all
			sprintf(rep_msg, "%s : %s", user_name, msg);
			for(i=0; i<(*num_clients); i++){
				if (fd_array[i].fd_client != fd)  /* on ne renvoie pas à l'emetteur */
				if ((result = write(fd_array[i].fd_client, rep_msg, strlen(rep_msg))) < 0) { perror("write ALL2"); exit(EXIT_FAILURE); }
			}
			break;
		} //switch

	} else {
		printf("--- End of connection of client %d as %s\n", user_id, user_name);  // a modifier avec le pseudo du mec
		exitClient(fd, readfds, fd_array, num_clients);
	} //if read

	free(user_name);

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
  server_address.sin_port = htons(get_Config("Port"));
  if (bind(*server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("bind"); exit(EXIT_FAILURE); }
  if (getsockname(*server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("getsockname"); exit(EXIT_FAILURE); }

  printf("\n*** Connections available on port : %d ***\n", ntohs(server_address.sin_port));

  return server_sockfd;
}

void routine_server(int * server_sockfd){

  int i=0;
  int num_clients = 0;
  int client_sockfd;
  struct sockaddr_in client_address;
  int addresslen = sizeof(struct sockaddr_in);
  int fd; //desc quand un client parle
  client_data fd_array[MAX_CLIENTS]; //tableau de data client
  fd_set readfds, testfds;
  int maxfds;
  char msg[WRITE_SIZE];
  char rep_msg[MSG_SIZE];

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

            login_client(&client_sockfd, fd_array, &num_clients, &readfds);

          } else {
            printf("--- Someone tried to connect, but too many clients online\n");
            sprintf(msg, "X[SERVER] : Sorry, too many clients online.  Try again later.\n"); // modifier le X pour dire au client de couper
            write(client_sockfd, msg, strlen(msg));
            close(client_sockfd);
          } //if num_clients < MAX_CLIENTS

        } else if (fd == 0) {  /*activité sur le clavier*/
          fgets(msg, WRITE_SIZE, stdin);
          if (strcmp(msg, "quit\n")==0) {      // A arranger avec plus de tests : si longueur 4 et quit ou des trucs du genre
            quit_server(&readfds, fd_array, server_sockfd, &num_clients);
          } else {
            sprintf(rep_msg, "[SERVER] : %s", msg);
            for (i=0; i<num_clients ; i++)
            write(fd_array[i].fd_client, rep_msg, strlen(rep_msg));
          }

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

void login_client(int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds){

  char msg[WRITE_SIZE];
  char user[WRITE_SIZE];
	char pswd[WRITE_SIZE] = "***"; // Provisoire
  int result = -1;

  sprintf(msg, "[SERVER] : Please log in : \n");
  write(*client_sockfd, msg, strlen(msg));

  do{
    memset (user, '\0', sizeof (user));//réinitialisation chaine
    if ((result = read(*client_sockfd, user, WRITE_SIZE)) <= 0){ //si le client se déco au moment du login
		  result = -1;
      break;
    }else if(strlen(user) > 15){
      sprintf(msg, "[SERVER] : Username too long, please enter another: \n");
      write(*client_sockfd, msg, strlen(msg));
    }else if(strlen(user) < 3){
      sprintf(msg, "[SERVER] : Username too short, please enter another: \n");
      write(*client_sockfd, msg, strlen(msg));
    }
  }while(strlen(user) > 15 || strlen(user) < 3);

  if(result != -1){
    fd_array[*num_clients].fd_client=*client_sockfd;
    user[result-1] = '\0'; //-1 sinon on remplace déjà le '\0' pr '\0', ici je le met a la place du '\n'
    if(exist_user_mysql(user)){
			fd_array[*num_clients].id_client=id_user_mysql(user);
      sprintf(msg, "[SERVER] : Welcom back %s ! User id is : %d\n", user, fd_array[*num_clients].id_client);
      write(*client_sockfd, msg, strlen(msg));
    }else{
      add_user_mysql(user, pswd);
			fd_array[*num_clients].id_client=id_user_mysql(user);
      sprintf(msg, "Oh, you'r new %s ! User id is : %d\n", user, fd_array[*num_clients].id_client);
      write(*client_sockfd, msg, strlen(msg));
    }
		(*num_clients)++;
    printf("--- Client %d joined as : %s\n",fd_array[*num_clients].id_client, user);///*** a modif avec le pseudo du mec
  }else{
    close(*client_sockfd);
		FD_CLR(*client_sockfd, readfds);
  }
}

int search_client(int fd, client_data *fd_array, int *num_clients){

	int i;

	for(i=0; i<*num_clients; i++){
		if(fd == fd_array[i].fd_client)
			return fd_array[i].id_client;
	}

	return -1; //pas exit failure car fct qui peut surement resservir

}
