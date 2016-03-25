#include "serv_function.h"

void ask_name(){

	/*Fonction qui demande le nom de l'utilisateur, a compléter plus tard avec un système
	d'autentification plus complexe.*/

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

void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients){

	/*Permet d'enlever les données d'un client du tableau fd_array quand
	celui-i se déconnecte ou subit une déconnexion.*/

	int i;
	int index;
	close(fd);
	FD_CLR(fd, readfds); //on enlève le leaver du tableau de clients
	//On cherche le FD du client qui se deconnecte
	for (i = 0; i < (*num_clients); i++){
		index = search_client_array_by_fd(fd, fd_array, num_clients);
	}
	//On decale les fd superieur a celui qui se connecte
	for (i=index; i < (*num_clients) - 1; i++){
		fd_array[i] = fd_array[i + 1];
	}
	(*num_clients)--;
}

void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients){

	/*Fonction qui envois le message de déconnexion quand on tape /quit,
	a tous les clients.*/

	int i;
	message *msg = (message *) malloc(sizeof(message));

	session_end(msg);
	for (i = 0; i < *num_clients ; i++) {
		send_msg(msg, &fd_array[i].fd_client,readfds,fd_array,num_clients);
		close(fd_array[i].fd_client);
	}
	free((*msg).msg_content);
	free(msg);
	close(*server_sockfd);
	printf("[Program] Goodbye !\n");
	*server_sockfd = -1; //On place a -1 pour sortir de la boucle de routine_server.
}

void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients) {

	/*Fonction qui lit les messages en attentes sur le file descriptor*/

	char msg[WRITE_SIZE];
	int  result;

	memset(msg, '\0', WRITE_SIZE);

	if ((result = read(fd, msg, MSG_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		//printf("msg recu : %s\n", msg); //pour debug
		rechercheProtocol(msg, &fd, fd_array, num_clients, readfds);
	} else {
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

	/*Initialise le serveur d'écoute du programme.*/

	int optval = 1;
	struct sockaddr_in server_address;
	int addresslen = sizeof(struct sockaddr_in);
	int * server_sockfd = (int *) malloc(sizeof(int));

	printf("\n*** Server program starting (enter \"/quit\" to stop) ***\n");
	*server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*server_sockfd < 0){
		perror("socket server");
		exit(EXIT_FAILURE);
	}

	if(setsockopt(*server_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
		perror("setsockopt() error: Re-use address");
		close(*server_sockfd);
		exit(EXIT_FAILURE);
	}

	if(setsockopt(*server_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
		perror("setsockopt() error: Keep Alive");
		close(*server_sockfd);
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = 0;

	if (bind(*server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("bind"); exit(EXIT_FAILURE); }
	if (getsockname(*server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("getsockname"); exit(EXIT_FAILURE); }

	printf("\n*** Connections available on port : %d ***\n", ntohs(server_address.sin_port));

	return server_sockfd;
}

void routine_server(int * server_sockfd){

	/*La routine du programme.*/

  int num_clients = 0;
	int client_sockfd;
	struct sockaddr_in client_address;
	int addresslen = sizeof(struct sockaddr_in);
	int fd; //desc quand un client parle
	client_data fd_array[MAX_CLIENTS]; //tableau de data client
	fd_set readfds, testfds;
	int maxfds;
	message *msg = (message *) malloc(sizeof(message));

	maxfds = *server_sockfd;
	if(listen(*server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???
	FD_ZERO(&readfds);
	FD_SET(*server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

	/*  Attente de clients et de requêtes */
	while (*server_sockfd != -1) {
		testfds = readfds;
		if(select(maxfds+1, &testfds, NULL, NULL, NULL) < 0) {
			perror("Select");
			exit(EXIT_FAILURE);
		}

		/* Il y a une activité, on cherche sur quel descripteur grâce à FD_ISSET */
		for (fd=0; fd<maxfds+1; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				/* Accept des nouvelles connections */
				if (fd == *server_sockfd) {
					if((client_sockfd = accept(*server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&addresslen )) < 0 ) {
						perror("Accept");
						exit(EXIT_FAILURE);
					}

					//Si on peut recevoir le client
					if (num_clients < MAX_CLIENTS) {
						//On rajoute le client en optimisant les descripteurs
						opt_desc(&client_sockfd, &maxfds, &readfds);
					//Sinon on le refuse
					} else {
						printf("[Program] Someone tried to connect, but too many clients online.\n");
						session_denied(msg, 0);
						send_msg(msg, &client_sockfd,&readfds,fd_array,&num_clients);
						free((*msg).msg_content);
						close(client_sockfd);
					} //if num_clients < MAX_CLIENTS

					} else if (fd == 0) {  /*activité sur le clavier*/
						cmde_host(&readfds, server_sockfd, &maxfds, fd_array, &num_clients);

					} else {  /*activité d'un client*/
            	traiterRequete(fd, &readfds, fd_array, &num_clients);
          }//if fd ==
				/* DEBUG
				int i;
				printf("FD_array\n");
				for (i = 0; i < num_clients; i++) {
					printf("pos:%d fd:%d id:%d name:%s rdy:%d\n",i,fd_array[i].fd_client,fd_array[i].id_client,fd_array[i].name_client,fd_array[i].rdy );

				}
				printf("\n");
				*/
	    }//if FD_ISSET
	  }//for
	}//while

  pthread_join(pid_transfer, NULL);
  // peut être mettre une variable et faire un pthread_cancel(pid_transfer) si transfer pas terminé

  free(msg);
  free(server_sockfd);
}

void cmde_host(fd_set *readfds, int *server_sockfd, int *maxfds, client_data *fd_array, int *num_clients){

	/*Fonction qui gère les données entrées au clavier par l'utilisateur.*/

	char msg[WRITE_SIZE];
	char rep_msg[MSG_SIZE];
	int i;
	message *frame = (message *) malloc(sizeof(message));

	fgets(msg, WRITE_SIZE, stdin);
	if (strcmp(msg, "/quit\n")==0) {      // A arranger avec plus de tests : si longueur 4 et quit ou des trucs du genre
		quit_server(readfds, fd_array, server_sockfd, num_clients);
	} else if (strcmp(msg, "/connect\n")==0){
		client(maxfds, readfds, num_clients, fd_array, NULL);
	} else if (strncmp(msg, "/connect", 8)==0){ //cas ou on met un contact après
		connect_to_contact(maxfds, readfds, num_clients, fd_array, msg);
	} else if (strcmp(msg, "/add\n") == 0){
		update_contact();
	} else if (strcmp(msg, "/remove\n") == 0){
		remove_contact();
	} else if (strcmp(msg, "/contact\n") == 0){
		print_contact_list();
  } else if (strcmp(msg, "/transfer\n")==0){
        init_transfer(4, readfds, fd_array, num_clients);
	} else {
		/*Faire une fonction plus poussée pour cette partie.*/

		normal_msg(frame, msg);
		for (i=0; i<*num_clients ; i++) {
			send_msg(frame, &(fd_array[i].fd_client),readfds,fd_array,num_clients);
		}
		free((*frame).msg_content);
		free(frame);
	}
}
