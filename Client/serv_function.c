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
	char client_inaddr[INET_ADDRSTRLEN];
	message *msg = (message *) malloc(sizeof(message));

	maxfds = *server_sockfd;
	if(listen(*server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???
	FD_ZERO(&readfds);
	FD_SET(*server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

	/*  Attente de clients et de requêtes */
	while (*server_sockfd != -1) {
		testfds = readfds;
		printf("maxfds : %i\n", maxfds);
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

					/* Ajout de l'adresse du client qui se connecte à nous à ses données */
					inet_ntop(AF_INET, &(client_address.sin_addr), client_inaddr, INET_ADDRSTRLEN);
					memset(fd_array->address_client, '\0', sizeof(fd_array->address_client));
					strcpy(fd_array->address_client, client_inaddr);

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
	char * contact;
	int i;

	fgets(msg, WRITE_SIZE, stdin);
	if (strcmp(msg, "/quit\n")==0) {
		quit_server(readfds, fd_array, server_sockfd, num_clients);
	} else if (strcmp(msg, "/help\n")==0 || strncmp(msg, "/help", 5)==0) {
		help(msg);
	} else if (strcmp(msg, "/connect\n")==0){
		client(maxfds, readfds, num_clients, fd_array, NULL);
	} else if (strncmp(msg, "/connect", 8)==0){ //cas ou on met un contact après
		connect_to_contact(maxfds, readfds, num_clients, fd_array, msg);
	} else if (strncmp(msg, "/disconnect", 11)==0){ //on précise avec qui on se déconnecte
		disconnect(maxfds, readfds, num_clients, fd_array, msg);
	} else if (strncmp(msg, "/msg", 4)==0) {
		slash_msg(msg, readfds, fd_array, num_clients);
	} else if (strncmp(msg, "/all", 4)==0) {
		slash_all(0, msg, readfds, fd_array, num_clients);
	} else if (strcmp(msg, "/add\n") == 0){
		update_contact();
	} else if (strncmp(msg, "/add", 4) == 0){ //cas où on met un contact après
		add_contact_online(fd_array, num_clients, msg);
	} else if (strcmp(msg, "/remove\n") == 0){
		remove_contact();
	} else if (strcmp(msg, "/contact\n") == 0){
		print_contact_list();
	} else if (strcmp(msg, "/who\n") == 0) {
		print_connected_user(fd_array, num_clients);
	} else if (strcmp(msg, "/transfer\n")==0){
    init_transfer(4, readfds, fd_array, num_clients); // changer le 4 avec le fd du mec
	} else {
		slash_all(1, msg, readfds, fd_array, num_clients);
	}
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
		printf("[PROGRAM] Wrong argument : /msg name, length of name must be between 3 and 16\n");
		free(frame);
		return;
	} else if (strlen(cmd) > WRITE_SIZE) { //on va éviter qu'il puisse écrire  l'infini hein
		printf("[PROGRAM] Argument too long\n");
		free(frame);
		return;
	}

	w=my_count_word(cmd+i);
	while(w>0) {
		sscanf(cmd+i, "%s", username);
		if ((fd[cptfd] = search_client_fd_by_name(username, fd_array, num_clients)) == -1) {
			printf("[PROGRAM] %s not connected\n", username);
			printf("[PROGRAM] /msg aborted\n");
			free(frame);
			return;
		}
		i+=strlen(username);
		w--;
	}
	printf("Enter your message :\n");
	fgets(msg, WRITE_SIZE, stdin);
	normal_msg(frame, msg);
	int fds;
	fds = fd[cptfd];
	send_msg(frame, &fds, readfds, fd_array, num_clients);
	free((*frame).msg_content);
	free(frame);
}

void slash_all(int mod, char *cmd, fd_set *readfds, client_data *fd_array, int *num_clients) {
	char msg[WRITE_SIZE];
	int i;
	message *frame = (message *) malloc(sizeof(message));
	if (mod == 0) {
		if (strcmp(cmd, "/all\n")==0) {
			printf("Enter your message :\n");
			fgets(msg, WRITE_SIZE, stdin);
		} else if (strlen(cmd) > 6) { //6 car strlen("/all \n") = 6 = message vide
			strcpy(msg, cmd+5);
		} else {
			printf("[PROGRAM] Wrong argument : /all + ENTER or /all + your message\n");
			free(frame);
			return ;
		}
		normal_msg(frame, msg);
		for (i=0; i<*num_clients ; i++) {
			send_msg(frame, &(fd_array[i].fd_client),readfds,fd_array,num_clients);
		}
	} else { //mod = 1
		normal_msg(frame, cmd);
		for (i=0; i<*num_clients ; i++) {
			send_msg(frame, &(fd_array[i].fd_client),readfds,fd_array,num_clients);
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

int help(char * msg) {

	/*Fonction qui affiche l'aide des fonctions demandées par l'utilisateur.*/

  char * posSpace = NULL;
  printf("\n\t[PROGRAM]\n\n");
  if((posSpace = strchr(msg, '\n')) == NULL) {
    printf("The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\nUse : /help FunctionName\n");
		return;
  }
	if((posSpace = strchr(msg, ' ')) == NULL) {
		printf("The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\nUse : /help FunctionName\n");
		return;
	}
	if (posSpace[0] == ' ') {
    memmove(posSpace, posSpace+1, strlen(posSpace));
	}
	if (strcmp(posSpace, "quit\n")==0) {
		printf("The quit function allows you to quit the chat. It will close all connections.\nUse : \"/quit\"\n");
	} else if (strcmp(posSpace, "connect\n")==0){
		printf("The connect function allows you to establish a connection with another user.\nUse : \"/connect\" to connect to a user you don't already have in your contact list. You will be ask his/her socket address (address port).\nUse : \"/connect USERNAME\" to connect to a user you already have in your contact list.\n");
	} else if (strcmp(posSpace, "msg\n")==0) {
    printf("The msg function allows you to send a direct message to other users.\nUse : \"/msg USERNAME ...\" to send a direct message to one or more users.\nThen you will be asked to type your message and press [Enter] to send it.\n");
	} else if (strcmp(posSpace, "all\n")==0) {
    printf("The all function allows you to send a message to everyone you're connected with.\nUse : \"/all Message Written Here\"\n  OR\nUse : \"/all\", press [Enter], then you will be asked to type your message and press [Enter] again to send it.\n");
	} else if (strcmp(posSpace, "add\n") == 0){
    printf("The add function allows you to add a user to your contact list or update one who is already in it.\nYou can either use \"/add USERNAME\" to add a user you're connected to\nor \"/add\", press [Enter] and complete his/her name and address.\n");
	} else if (strcmp(posSpace, "remove\n") == 0){
    printf("The remove function allows you to remove a user from your contact list.\nUse : \"/remove\"\nYou will be asked to type the contact's username so as to remove its data.\n");
	} else if (strcmp(posSpace, "contact\n") == 0){
    printf("The contact function allows you to print your contact list.\nUse : \"/contact\"\n");
	} else if (strcmp(posSpace, "who\n") == 0) {
    printf("The who function allows you to print every user you are currently connected to.\nUse : \"/who\"\n");
	} else if (strcmp(posSpace, "transfer\n")==0){
    printf("The transfer function allows you to transfer text files and binary files.\nUse :\"/transfer\", then type the filename (absolute or relative) and press [Enter].\nThe transfer will start and a message will be displayed upon success or failure.\n");
	} else {
    printf("The help function print help for functions : quit, connect, msg, all, add, remove, contact, who, transfer\nUse : /help FunctionName\n");
	}
	return;
}
