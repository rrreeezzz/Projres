#include "serv_function.h"

void ask_name(){

	/*Fonction qui demande le nom de l'utilisateur, a compléter plus tard avec un système
	d'autentification plus complexe.*/

	char user[WRITE_SIZE]; //laisser a write size pour que ce soit cohérent avec le read
	int result;

	printf(BLUE"[PROGRAM] Please enter your name: "RESET"\n");
	do {
		memset (user, '\0', sizeof(user));//réinitialisation chaine
		if ((result = read(0, user, WRITE_SIZE)) <= 0){
			perror("Name read error.");
			exit(EXIT_FAILURE);
			break;
		} else if(result > 16) { //on test result car sinon bug si l'utilisateur rentre + que 15, et > 16 car result compte le \n
			printf(BLUE"[PROGRAM] Username too long, please enter another: "RESET"\n");
		} else if(result < 4) {
			printf(BLUE"[PROGRAM] Username too short, please enter another: "RESET"\n");
		}
		if (strchr(user, ' ') != NULL) {
			printf(BLUE"[PROGRAM] You can't have a space in your username, please enter another : "RESET"\n");
		}
	} while(result > 16 || result < 4 || strchr(user, ' ') != NULL);

	user[strlen(user)-1] = '\0';
	strcpy(General_Name, user);

	printf(BLUE"Your name is: "RED"%s"RESET"\n", General_Name);

}

void exitClient(int fd, fd_set *readfds, client_data *fd_array, int *num_clients){

	/*Permet d'enlever les données d'un client du tableau fd_array quand
	celui-i se déconnecte ou subit une déconnexion.*/


	//On gere le cas ou c'est l'interface de deconnecte
	if (fd==userInterface_fd){
		//On avertis l'utilisateur
		printf(BLUE"["GREEN"UI"BLUE"] End of connection."RESET"\n");
		userInterface_fd=-1;
	} else {
		//On avertis l'utilisateur
		printf(BLUE"["RED"%s"BLUE"] End of connection."RESET"\n", fd_array[search_client_array_by_fd(fd, fd_array, num_clients)].name_client);
	}

	//on avertis l'ui si elle est connectee
	if (userInterface_fd > 0) {
		message *msg = (message *) malloc(sizeof(message));
		char content[MSG_SIZE];
		printf("%s\n",fd_array[fd].name_client);
		sprintf(content,"DISCONNECTCONFIRM %s \n",fd_array[search_client_array_by_fd(fd, fd_array, num_clients)].name_client);
		normal_msg(msg,content);
		send_msg(msg, &userInterface_fd ,readfds,fd_array,num_clients);
		free((*msg).msg_content);
		free(msg);
	}

	int i;
	int index;
	close(fd);

	FD_CLR(fd, readfds); //on enlève le leaver du tableau de clients
	//On cherche le FD du client qui se deconnecte
	for (i = 0; i < (*num_clients); i++){
		index = search_client_array_by_fd(fd, fd_array, num_clients);
	}

	//On decale les fd superieur a celui qui se connecte
	for (i=index; i < (*num_clients) ; i++){
		fd_array[i] = fd_array[i + 1];
	}
	(*num_clients)--;
}

void quit_server(fd_set *readfds, client_data *fd_array, int *server_sockfd, int *num_clients){

	/*Fonction qui envoie le message de déconnexion quand on tape /quit,
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
	printf(BLUE"[Program] Goodbye !"RESET"\n");
	*server_sockfd = -1; //On place a -1 pour sortir de la boucle de routine_server.
}

void traiterRequete(int fd, fd_set *readfds, client_data *fd_array, int *num_clients, waitList *waitlist) {

	/*Fonction qui lit les messages en attentes sur le file descriptor*/

	char msg[MSG_SIZE];
	int  result;

	memset(msg, '\0', MSG_SIZE);

	if ((result = read(fd, msg, MSG_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		//printf("msg recu : %s\n", msg); //pour debug
		rechercheProtocol(msg, &fd, fd_array, num_clients, readfds, waitlist);
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

	//Pas d'interface utilisateur au debut
	int userInterface_fd = -1;

	printf(BLUE"\n*** Server program starting (enter \"/quit\" to stop) ***"RESET"\n");
	*server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (*server_sockfd < 0){
		perror("socket server");
		exit(EXIT_FAILURE);
	}

	// Pour réutiliser le port à la fin du programme
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

	printf(BLUE"\n*** Connections available on port : "RED"%d"BLUE" ***"RESET"\n", ntohs(server_address.sin_port));

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
	char client_inaddr[INET_ADDRSTRLEN];
	message *msg = (message *) malloc(sizeof(message));

	/* Initialisation de la file d'attente */
	waitList waitlist;
	waitlist.nb_connect = 0;

	maxfds = *server_sockfd;
	if(listen(*server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???
	FD_ZERO(&readfds);
	FD_SET(*server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

	/*  Attente de clients et de requêtes */
	while (*server_sockfd != -1) {
		testfds = readfds;
		//printf("maxfds : %i\n", maxfds); //debug
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
						printf(BLUE"[Program] Someone tried to connect, but too many clients online."RESET"\n");
						session_denied(msg, 0);
						send_msg(msg, &client_sockfd,&readfds,fd_array,&num_clients);
						free((*msg).msg_content);
						close(client_sockfd);
					} //if num_clients < MAX_CLIENTS

					/* Ajout de l'adresse et de la socket du client qui se connecte à nous à ses données pré-enregistrées */
					(waitlist.nb_connect)++;
					inet_ntop(AF_INET, &(client_address.sin_addr), client_inaddr, INET_ADDRSTRLEN);
					memset(fd_array[num_clients+waitlist.nb_connect].address_client, '\0', sizeof(fd_array->address_client));
					strcpy(fd_array[num_clients+waitlist.nb_connect].address_client, client_inaddr);
					waitlist.waiting[waitlist.nb_connect]=client_sockfd;
					fd_array[num_clients+waitlist.nb_connect].fd_client = client_sockfd;
				} else if (fd == 0 || fd == userInterface_fd) {  /*activité sur le clavier*/
					cmde_host(fd,&readfds, server_sockfd, &maxfds, fd_array, &num_clients, &waitlist);
				} else {  /*activité d'un client*/
					printf("%d\n",userInterface_fd);
          traiterRequete(fd, &readfds, fd_array, &num_clients, &waitlist);
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

void cmde_host(int fd,fd_set *readfds, int *server_sockfd, int *maxfds, client_data *fd_array, int *num_clients, waitList *waitlist){

	/*Fonction qui gère les données entrées au clavier par l'utilisateur.*/

	char msg[WRITE_SIZE];
	int countdisc;
	int ch;

	if (fd == 0){
		fgets(msg, WRITE_SIZE, stdin);
	} else { //Si le message viens de l'ui

		char buffer[MSG_SIZE];

		//On lis le message, sinon on quitte la connection en resettant le fd de l'interface
		if ( read(fd, buffer, MSG_SIZE) < 0 ){
			exitClient(fd, readfds, fd_array, num_clients);
			userInterface_fd=-1;
			return ;
		}

		message *msg_rcv = (message *) malloc(sizeof(message));

		printf("%s\n", buffer );

		//On parse le message
		if(protocol_parser(buffer, msg_rcv) == -1){
			free(msg_rcv);
			printf(GREEN"Error parsing the message\n"RESET);
			return ;
		}

		//protocole de communication avec l'application, on reimplente uniquement les fonctions basiques
		switch((*msg_rcv).code) {

			// 100 : Basic command
			case 100:
				printf(BLUE"["GREEN"UI"BLUE"] %s "RESET"\n", (*msg_rcv).msg_content);
				memcpy(msg,(*msg_rcv).msg_content,WRITE_SIZE);
				break;

			// 303 : SESSION_END
			case 303:
				//On deconnecte l'utilisateur
				exitClient(userInterface_fd, readfds, fd_array, num_clients);

				//On execute default

			//On ne traite pas la commande
			default:

				//On libere le message
				free(msg_rcv);
				free(msg_rcv->msg_content);

				//On sors de la fonction
				return ;

		}
		free(msg_rcv->msg_content);
		free(msg_rcv);
	}

	//printf(CYAN"%s\n"RESET,msg );

	if(NULL == strchr(msg, '\n')){
		printf("[PROGRAM] : Message too long, max is %d caracters.\n", WRITE_SIZE);
		while((ch = getchar()) != '\n'){
			if(ch < 0) {
				perror("Erreur taille message");
			}
		}
	} else {

		if (strcmp(msg, "/quit\n")==0) {
			quit_server(readfds, fd_array, server_sockfd, num_clients);
		} else if (strcmp(msg, "/help\n")==0 || strncmp(msg, "/help", 5)==0) {
			help(msg);
		} else if (strcmp(msg, "/connect\n")==0){
			client(maxfds, readfds, num_clients, fd_array, NULL, waitlist);
		} else if (strncmp(msg, "/connect", 8)==0){ //cas ou on met un contact après
			connect_to_contact(maxfds, readfds, num_clients, fd_array, msg, waitlist);
		} else if (strncmp(msg, "/accept", 7)==0){ // /accept fd
			connect_accept(fd_array, num_clients, readfds, msg, waitlist);
		} else if (strncmp(msg, "/refuse", 7)==0){ // /refuse fd
			connect_refuse(fd_array, num_clients, readfds, msg, waitlist);
		} else if (strncmp(msg, "/disconnect", 11)==0){ //on précise avec qui on se déconnecte
			disconnect(maxfds, readfds, num_clients, fd_array, msg);
		} else if (strncmp(msg, "/msg", 4)==0) {
			slash_msg(msg, readfds, fd_array, num_clients);
		} else if (strncmp(msg, "/all", 4)==0) {
			slash_all(0, msg, readfds, fd_array, num_clients);
		} else if (strncmp(msg, "/add", 4) == 0){ // /add username address:port ou /add username (online)
			add_contact(fd_array, num_clients, msg);
		} else if (strncmp(msg, "/remove", 7) == 0){
			remove_contact(msg);
		} else if (strcmp(msg, "/contact\n") == 0){
			print_contact_list();
		} else if (strcmp(msg, "/who\n") == 0) {
			print_connected_user(fd_array, num_clients);
		} else if (strncmp(msg, "/transfer", 9)==0){
			slash_transfer(msg, readfds, fd_array, num_clients);
		} else if (strcmp(msg, "/online\n")==0){
			connect_serv();
		} else if (strncmp(msg, "/search", 7)==0){
			search_serv(msg, fd_array, num_clients, readfds, waitlist);
		} else {
			if(*num_clients > 0) {
				slash_all(1, msg, readfds, fd_array, num_clients);
			} else {
				if(countdisc < 10) { printf("[PROGRAM] You are not connected to anyone. You may have been disconnected from peer.\n\t  Use /who to double-check your connections\n\t  Use /connect to establish a connection or type /help if you need information about how this chat works.\n"); countdisc++;}
			}
		}
	}
}

void slash_transfer(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char username[16];
	int client_fd = 0;
	if (strlen(cmd) < 9) { //9 car strlen("/msg \n") = 6, et name entre 3 et 16 char, donc entre 9 minimum
		printf(BLUE"[PROGRAM] Wrong argument : /msg name, length of name must be between 3 and 16"RESET"\n");
		return;
	} else if (strlen(cmd) > WRITE_SIZE) { //on va éviter qu'il puisse écrire à l'infini hein
		printf(BLUE"[PROGRAM] Argument too long"RESET"\n");
		return;
	}
	sscanf(cmd+10, "%s", username); // +10 car cmd+10 correspond aux arguments (noms d'utilisateurs)
	if ((client_fd = search_client_fd_by_name(username, fd_array, num_clients)) == -1) {
		printf(BLUE"[PROGRAM] "RED"%s "BLUE"not connected"RESET"\n", username);
		printf(BLUE"[PROGRAM] /msg aborted"RESET"\n");
		return;
	}
	init_transfer(client_fd, readfds, fd_array, num_clients);
}

void slash_msg(char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char username[16];
	int fd[MAX_CLIENTS]={-1};
	char msg[WRITE_SIZE];
	int i=5; // 5 car cmd+5 correspond aux arguments (noms d'utilisateurs)
	int w=0;
	int cptfd=0;
	message *frame = (message *) malloc(sizeof(message));

	if (strlen(cmd) < 9) { //9 car strlen("/msg \n") = 6, et name entre 3 et 16 char, donc entre 9 minimum
		printf(BLUE"[PROGRAM] Wrong argument : /msg name, length of name must be between 3 and 16"RESET"\n");
		free(frame);

		//on avertis l'ui si elle est connectee
		if (userInterface_fd > 0 ) {
			sendUiMsg("MESSAGEERROR Wrong argument\n",readfds,fd_array,num_clients);
		}

		return;

	} else if (strlen(cmd) > WRITE_SIZE) { //on va éviter qu'il puisse écrire à l'infini hein
		printf(BLUE"[PROGRAM] Argument too long"RESET"\n");
		free(frame);

		//on avertis l'ui si elle est connectee
		if (userInterface_fd > 0 ) {
			sendUiMsg("MESSAGEERROR Argument too long\n",readfds,fd_array,num_clients);
		}

		return;

	}

	w=my_count_word(cmd); //compte le nombre d'arg après /msg

	while(w>0) {
		sscanf(cmd+i, "%s", username);
		if ((fd[cptfd] = search_client_fd_by_name(username, fd_array, num_clients)) == -1) {
			printf(BLUE"[PROGRAM] "RED"%s"BLUE" not connected"RESET"\n", username);
			printf(BLUE"[PROGRAM] /msg aborted"RESET"\n");
			free(frame);

			//on avertis l'ui si elle est connectee
			if (userInterface_fd > 0 ) {
				sendUiMsg("MESSAGEERROR Client not connected\n",readfds,fd_array,num_clients);
			}

			return;
		}
		cptfd++;
		i+=strlen(username);
		w--;
	}
	printf(BLUE"Enter your message :"RESET"\n");
	fgets(msg, WRITE_SIZE, stdin);
	normal_msg(frame, msg);
	int fds;
	for(cptfd--;cptfd>=0;cptfd--) {
		fds = fd[cptfd];
		send_msg(frame, &fd[cptfd], readfds, fd_array, num_clients);
	}

	//on avertis l'ui si elle est connectee
	if (userInterface_fd > 0 ) {
		char content[MSG_SIZE];
		sprintf(content,"MESSAGECONFIRM %s %s\n",username,msg);
		sendUiMsg(content,readfds,fd_array,num_clients);
	}

	free((*frame).msg_content);
	free(frame);
}

void slash_all(int mod, char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char msg[WRITE_SIZE];
	int i;
	message *frame = (message *) malloc(sizeof(message));
	char *posSpace = NULL;

	if (mod == 0) {
		if (strcmp(cmd, "/all\n")==0) {
			printf(BLUE"[PROGRAM] Error command. Please use \"/all message\"."RESET"\n");

			//on avertis l'ui si elle est connectee
			if (userInterface_fd > 0 ) {
				sendUiMsg("ALLMSGERROR Command Error\n",readfds,fd_array,num_clients);
			}

			return ;

		} else if (strlen(cmd) > 6) { //6 car strlen("/all \n") = 6 = message vide
			strcpy(msg, cmd+5);
		} else {
			printf(BLUE"[PROGRAM] Error command. Please use \"/all message\"."RESET"\n");

			//on avertis l'ui si elle est connectee
			if (userInterface_fd > 0 ) {
				sendUiMsg("ALLMSGERROR Command Error\n",readfds,fd_array,num_clients);
			}

			free(frame);
			return ;

		}
		normal_msg(frame, msg);
		for (i=0; i<*num_clients ; i++) {
			if (fd_array[i].fd_client != userInterface_fd){
				send_msg(frame, &(fd_array[i].fd_client),readfds,fd_array,num_clients);
			}
		}
		//on avertis l'ui si elle est connectee
		if (userInterface_fd > 0 ) {
			char message[MSG_SIZE];
			sprintf(message,"ALLMSGCONFIRM %s \n",msg);
			sendUiMsg(message,readfds,fd_array,num_clients);
		}
	} else { //mod = 1
		normal_msg(frame, cmd);
		for (i=0; i<*num_clients ; i++) {
			if (fd_array[i].fd_client != userInterface_fd){
				send_msg(frame, &(fd_array[i].fd_client),readfds,fd_array,num_clients);
			}
		}
		//on avertis l'ui si elle est connectee
		if (userInterface_fd > 0 ) {
			char message[MSG_SIZE];
			sprintf(message,"ALLMSGCONFIRM %s \n",cmd);
			sendUiMsg(message,readfds,fd_array,num_clients);
		}
	}

	free((*frame).msg_content);
	free(frame);
}

int is_sep(char c) {
  if (c == ' ' || c == '\0' || c == '\t')
    return (1);
  return (0);
}

int my_count_word(const char *str) {
  int   count;
  int   word;

  if (str == NULL)
    return (0);
  word = 0;
  count = 0;
  while (str[count] != '\0')
    {
      if (is_sep(str[count]) && is_sep(str[count + 1]) == 0)
        ++word;
      ++count;
    }
  return (word);
}

void help(char * msg) {

	/*Fonction qui affiche l'aide des fonctions demandées par l'utilisateur.*/

 	char * posSpace = NULL;
	if((posSpace = strchr(msg, '\n')) == NULL) {
		printf(BLUE"\n[PROGRAM] Hello ! This is a client/server chat application. You need to connect you to other user to start chating\n\t  The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\n\t  Use : /help FunctionName"RESET"\n");
		return;
 	}
	if((posSpace = strchr(msg, ' ')) == NULL) {
		printf(BLUE"\n[PROGRAM] Hello ! This is a client/server chat application. You need to connect you to other user to start chating\n\t  The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\n\t  Use : /help FunctionName"RESET"\n");
		return;
	}
	if (posSpace[0] == ' ') {
    		memmove(posSpace, posSpace+1, strlen(posSpace));
	}
	if (strcmp(posSpace, "quit\n")==0) {
		printf(BLUE"\n[PROGRAM] The quit function allows you to quit the chat. It will close all connections.\n\t  Use : \"/quit\""RESET"\n");
	} else if (strcmp(posSpace, "connect\n")==0){
		printf(BLUE"\n[PROGRAM] The connect function allows you to establish a connection with another user.\n\t  Use : \"/connect\" to connect to a user you don't already have in your contact list. You will be ask his/her socket address (address port).\n\t  OR\n\t  Use : \"/connect USERNAME\" to connect to a user you already have in your contact list."RESET"\n");
	} else if (strcmp(posSpace, "msg\n")==0) {
    		printf(BLUE"\n[PROGRAM] The msg function allows you to send a direct message to other users.\n\t  Use : \"/msg USERNAME ...\" to send a direct message to one or more users.\n\t  Then you will be asked to type your message and press [Enter] to send it."RESET"\n");
	} else if (strcmp(posSpace, "all\n")==0) {
    		printf(BLUE"\n[PROGRAM] The all function allows you to send a message to everyone you're connected with.\n\t  Use : \"/all Message Written Here\"\n\t  OR\n\t  Use : \"/all\", press [Enter], then you will be asked to type your message and press [Enter] again to send it."RESET"\n");
	} else if (strcmp(posSpace, "add\n") == 0){
   		 printf(BLUE"\n[PROGRAM] The add function allows you to add a user to your contact list or update one who is already in it.\n\t  Use : \"/add USERNAME\" to add a user you're connected to\n\t  OR\n\t  Use : \"/add\", press [Enter] and complete his/her name and address."RESET"\n");
	} else if (strcmp(posSpace, "remove\n") == 0){
   		 printf(BLUE"\n[PROGRAM] The remove function allows you to remove a user from your contact list.\n\t  Use : \"/remove\"\n\t  You will be asked to type the contact's username so as to remove its data."RESET"\n");
	} else if (strcmp(posSpace, "contact\n") == 0){
   		 printf(BLUE"\n[PROGRAM] The contact function allows you to print your contact list.\n\t  Use : \"/contact\""RESET"\n");
	} else if (strcmp(posSpace, "who\n") == 0) {
   		 printf(BLUE"\n[PROGRAM] The who function allows you to print every user you are currently connected to.\n\t  Use : \"/who\""RESET"\n");
	} else if (strcmp(posSpace, "transfer\n")==0){
   		 printf(BLUE"\n[PROGRAM] The transfer function allows you to transfer text files and binary files.\n\t  Use : \"/transfer\", then type the filename (absolute or relative) and press [Enter].\n\t  The transfer will start and a message will be displayed upon success or failure."RESET"\n");
	} else {
   		 printf(BLUE"\n[PROGRAM] The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\n\t  Use : /help FunctionName"RESET"\n");
	}
}
